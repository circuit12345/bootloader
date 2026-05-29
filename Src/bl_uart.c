/*
 * bl_uart.c
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */
#include "main.h"
#include "bl_uart.h"
#include "ota_config.h"     /* brings OTA_MAX_SIZE, OTA_CHUNK_SIZE and timeouts */


uint8_t rcvd_size = 0;
/* legacy duplicates removed – configuration lives in ota_config.h */

static UART_HandleTypeDef *bl_active_uart = NULL;

UART_HandleTypeDef *BL_UART_GetHandle(void)
{
    return bl_active_uart;
}

BL_Status BL_UART_Read_WithTimeout(uint8_t *buf, uint32_t len, uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();
    uint32_t rx = 0;
    rcvd_size = 0;

    while (rx < len)
    {
        if (HAL_UART_Receive(bl_active_uart, &buf[rx], 1, 100) == HAL_OK)
        {
            rx++;
            rcvd_size = rx;
            start = HAL_GetTick(); // reset timeout on activity
        }

        if ((HAL_GetTick() - start) > timeout_ms)
        {
            return BL_TIMEOUT;
        }
    }
    return BL_OK;
}

//UART_Handl?eTypeDef huart4;

//void BL_UART_Init(void)
//{
//    bl_active_uart = &huart1;   // 👈 Change only here if needed
//
//    bl_active_uart->Instance = USART1;
//    bl_active_uart->Init.BaudRate = 115200;
//    bl_active_uart->Init.WordLength = UART_WORDLENGTH_8B;
//    bl_active_uart->Init.StopBits = UART_STOPBITS_1;
//    bl_active_uart->Init.Parity = UART_PARITY_NONE;
//    bl_active_uart->Init.Mode = UART_MODE_TX_RX;
//    bl_active_uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
//
//    HAL_UART_Init(bl_active_uart);
//}
void BL_UART_Init(void *uart)
{
    bl_active_uart = (UART_HandleTypeDef *)uart;

    bl_active_uart->Init.BaudRate   = 115200;
    bl_active_uart->Init.WordLength = UART_WORDLENGTH_8B;
    bl_active_uart->Init.StopBits   = UART_STOPBITS_1;
    bl_active_uart->Init.Parity     = UART_PARITY_NONE;
    bl_active_uart->Init.Mode       = UART_MODE_TX_RX;
    bl_active_uart->Init.HwFlowCtl  = UART_HWCONTROL_NONE;

    HAL_UART_Init(bl_active_uart);
}


int32_t BL_UART_Read(uint8_t *buf, uint32_t len, uint32_t timeout)
{
    return (int32_t)HAL_UART_Receive(bl_active_uart, buf, len, timeout);
}
void BL_UART_Read_Blocking(uint8_t *buf, uint32_t len)
{
    USART_TypeDef *U = bl_active_uart->Instance;
    volatile uint32_t sr;
    volatile uint32_t dr;

    for (uint32_t i = 0; i < len; i++)
    {
        /* Clear ORE if set */
        sr = U->SR;
        dr = U->DR;
        (void)sr;
        (void)dr;

        /* Wait for RXNE */
        while (!(U->SR & USART_SR_RXNE))
        {
            /* spin */
        }

        /* Read received byte */
        buf[i] = (uint8_t)(U->DR & 0xFF);
    }
}


void BL_UART_Write(uint8_t *buf, uint32_t len)
{
    HAL_UART_Transmit(bl_active_uart, buf, len, HAL_MAX_DELAY);
}

