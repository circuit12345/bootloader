/*
 * bl_update.c
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */

#include "bl_uart.h"
#include "bl_flash.h"
#include "crc32.h"
#include "flash_layout.h"
#include "bl_jump.h"
#include "ota_config.h"
#include "main.h"


#include "bl_ota_header.h"
#define OTA_TOTAL_TIMEOUT_MS   (30 * 1000)   // 30 seconds


static void UART_FlushRx(void)
{
    UART_HandleTypeDef *uart = BL_UART_GetHandle();
    volatile uint32_t tmp;
    while (__HAL_UART_GET_FLAG(uart, UART_FLAG_RXNE))
    {
        tmp = uart->Instance->DR;
        (void)tmp;
    }
}

void UART_Firmware_Update(void)
{
    UART_HandleTypeDef *uart = BL_UART_GetHandle();
	HAL_Delay(50);
	uint32_t ota_start_tick = HAL_GetTick();

    uint32_t app_size = 0;
    uint32_t app_crc  = 0;
    uint32_t calc_crc = 0;
    uint8_t  resp;
    uint8_t buf[OTA_CHUNK_SIZE];

    /* ---------- ACK BOOT ---------- */
    resp = 0xBB;
    HAL_UART_Transmit(uart, &resp, 1, 100);
    UART_FlushRx();
    
    /* ---------- Receive APP SIZE ---------- */
    if (BL_UART_Read_WithTimeout((uint8_t*)&app_size, 4, OTA_RX_TIMEOUT_MS) != BL_OK)
        goto OTA_ABORT;

    if (app_size == 0 || app_size > OTA_MAX_SIZE)
        goto OTA_ABORT;

    resp = 0xAD;
    HAL_UART_Transmit(uart, &resp, 1, 100);
    UART_FlushRx();

    /* ---------- Receive CRC ---------- */
    if (BL_UART_Read_WithTimeout((uint8_t*)&app_crc, 4, OTA_RX_TIMEOUT_MS) != BL_OK)
        goto OTA_ABORT;

    /* Prepare OTA header data in RAM only. Do not write the header to flash
       until the new image is fully received, verified, and committed to A. */
    ota_header_t hdr;
    hdr.magic              = OTA_HDR_MAGIC;
    hdr.image_size         = app_size;
    hdr.image_crc          = app_crc;
    hdr.state              = OTA_STATE_UPDATING;
    hdr.bootloader_version = BOOTLOADER_VERSION;
    hdr.partition_select   = PARTITION_A;  /* Currently running partition A */
    hdr.partition_b_crc    = 0;             /* Will set after verify */

    resp = 0xAB;
    HAL_UART_Transmit(uart, &resp, 1, 100);
    UART_FlushRx();

    /* ---------- ERASE PARTITION B ---------- */
    if (!Flash_Erase_Partition(PARTITION_B))
    {

        goto OTA_ABORT;
    }

    resp = 0x5A; // READY
    HAL_UART_Transmit(uart, &resp, 1, 100);
    UART_FlushRx();

    /* ---------- RECEIVE & PROGRAM TO PARTITION B ---------- */
    uint32_t addr = PART_B_APP_START;
    uint32_t remaining = app_size;

    while (remaining)
    {
        uint32_t len = (remaining > OTA_CHUNK_SIZE) ? OTA_CHUNK_SIZE : remaining;
        uint8_t retry = 0;

        while (retry < OTA_MAX_RETRY)
        {
            if (BL_UART_Read_WithTimeout(buf, len, OTA_CHUNK_TIMEOUT) == BL_OK)
                break;

            retry++;

            if ((HAL_GetTick() - ota_start_tick) > OTA_TOTAL_TIMEOUT_MS)
                goto OTA_ABORT;
        }

        if (retry == OTA_MAX_RETRY)
            goto OTA_ABORT;

        __disable_irq();
        Flash_Program(addr, buf, len);
        __enable_irq();
        
        HAL_UART_Transmit(uart, (uint8_t[]){0xAC}, 1, 100);
        UART_FlushRx();

        addr += len;
        remaining -= len;
    }

    /* ---------- CRC VERIFY PARTITION B ---------- */
    calc_crc = crc32((uint8_t*)PART_B_APP_START, app_size);
    if (calc_crc != app_crc)
    {
        /* CRC mismatch: Partition A will continue running on next boot */
        resp = 0xEE;
        HAL_UART_Transmit(uart, &resp, 1, 100);
        HAL_Delay(100);
        NVIC_SystemReset();
        return;
    }

    /* ---------- CRC VALID: COPY PARTITION B TO A ---------- */
    
    /* Erase partition A */
    if (!Flash_Erase_Partition(PARTITION_A))
    {
        goto OTA_ABORT;
    }

    /* Copy partition B to partition A (skip OTA header at start of A) */
    if (!Flash_Copy_Partition(PART_B_APP_START, PART_A_APP_START, app_size))
    {
        goto OTA_ABORT;
    }

    /* ---------- UPDATE HEADER: MARK AS VALID ---------- */
    hdr.state            = OTA_STATE_VALID;
    hdr.partition_select = PARTITION_A;  /* Running partition A */
    hdr.partition_b_crc  = calc_crc;
    Flash_Write_Header(&hdr);

    /* ---------- SUCCESS ---------- */
    resp = 0xCC;
    HAL_UART_Transmit(uart, &resp, 1, 100);
    HAL_Delay(50);
    JumpToApplication();
    return;

OTA_ABORT:
    resp = 0xEE;
    HAL_UART_Transmit(uart, &resp, 1, 100);
    HAL_Delay(100);
    NVIC_SystemReset();
}

