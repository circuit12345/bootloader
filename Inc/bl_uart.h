/*
 * bl_uart.h
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */

#ifndef BL_UART_H
#define BL_UART_H

#include <stdint.h>

typedef enum {
    BL_OK = 0,
    BL_TIMEOUT
} BL_Status;

typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

UART_HandleTypeDef *BL_UART_GetHandle(void);
void BL_UART_Init(void *uart);
void BL_UART_Read_Blocking(uint8_t *buf, uint32_t len);
int32_t BL_UART_Read(uint8_t *buf, uint32_t len, uint32_t timeout);
void BL_UART_Write(uint8_t *buf, uint32_t len);
BL_Status BL_UART_Read_WithTimeout(uint8_t *buf, uint32_t len, uint32_t timeout_ms);
#endif

