/*
 * boot_req.c
 *
 *  Created on: Jan 9, 2026
 *      Author: Sriram
 */

#include "boot_req.h"
#include "main.h"
#include "bl_uart.h"
#include "bl_update.h"
#include "bl_jump.h"
/* IMPORTANT:
 * - No initialization
 * - Goes to .bss
 * - Survives jump, cleared on power reset
 */
//volatile uint32_t ota_request_flag;

void OTA_RequestLoop(void)
{
    uint8_t req = 0x55;
    uint8_t resp;

    while (1)
    {
        /* Ask host for OTA */
        HAL_UART_Transmit(BL_UART_GetHandle(), &req, 1, 200);

        /* Wait for ACK from host */
        if (HAL_UART_Receive(BL_UART_GetHandle(), &resp, 1, 500) == HAL_OK)
        {
            if (resp == 0xAA)
            {
                /* Host accepted OTA */
                UART_Firmware_Update();
            }
        }

        HAL_Delay(500);
    }
}
