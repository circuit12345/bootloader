/*
 * bl_flash.c
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */



#include "bl_flash.h"

#include "flash_layout.h"
#include "ota_config.h"
#include "ota_flash_platform.h"

static inline void OTA_DisableIRQ(void)
{
    __asm volatile ("cpsid i" : : : "memory");
}

static inline void OTA_EnableIRQ(void)
{
    __asm volatile ("cpsie i" : : : "memory");
}

static uint32_t AlignDown(uint32_t value, uint32_t alignment)
{
    return value & ~(alignment - 1U);
}

static uint32_t AlignUp(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1U) & ~(alignment - 1U);
}

bool Flash_Erase_App(uint32_t size)
{
    if (size == 0U || size > OTA_MAX_SIZE)
        return false;

    uint32_t start_addr = APP_CODE_ADDR;
    uint32_t end_addr = start_addr + size - 1U;

    if (end_addr < start_addr)
        return false;

    if (end_addr > FLASH_END_ADDR)
        end_addr = FLASH_END_ADDR;

    uint32_t erase_start = AlignDown(start_addr, ota_flash_erase_block_size);
    uint32_t erase_end   = AlignUp(end_addr + 1U, ota_flash_erase_block_size);

    if (erase_start < APP_CODE_ADDR)
        return false;

    if (erase_end < erase_start)
        return false;

    uint32_t erase_size = erase_end - erase_start;

    return OTA_Flash_EraseRegion(erase_start, erase_size);
}

void Flash_Program(uint32_t addr, uint8_t *data, uint32_t len)
{
    (void)OTA_Flash_Program(addr, data, len);
}

void Flash_Write_Header(ota_header_t *hdr)
{
    (void)OTA_Flash_WriteHeader(hdr);
}

/* ============ PARTITION OPERATIONS ============ */

bool Flash_Erase_Partition(uint32_t partition)
{
    uint32_t start_addr;
    uint32_t size;

    if (partition == PARTITION_A)
    {
        start_addr = PART_A_START_ADDR;
        size = PART_A_SIZE;
    }
    else if (partition == PARTITION_B)
    {
        start_addr = PART_B_START_ADDR;
        size = PART_B_SIZE;
    }
    else
    {
        return false;
    }

    return OTA_Flash_EraseRegion(start_addr, size);
}

bool Flash_Copy_Partition(uint32_t src, uint32_t dst, uint32_t size)
{
    if (size == 0 || size > PART_B_SIZE)
        return false;

    uint8_t buffer[256];
    uint32_t remaining = size;
    uint32_t src_addr = src;
    uint32_t dst_addr = dst;

    while (remaining > 0)
    {
        uint32_t chunk_size = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;

        for (uint32_t i = 0; i < chunk_size; i++)
        {
            buffer[i] = *(volatile uint8_t *)(src_addr + i);
        }

        OTA_DisableIRQ();
        (void)OTA_Flash_Program(dst_addr, buffer, chunk_size);
        OTA_EnableIRQ();

        src_addr += chunk_size;
        dst_addr += chunk_size;
        remaining -= chunk_size;
    }

    return true;
}

bool Flash_Verify_Partition_CRC(uint32_t partition, uint32_t size, uint32_t expected_crc)
{
    (void)partition;
    (void)size;
    (void)expected_crc;

    return true;
}


