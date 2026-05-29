/*
 * bl_jump.c
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */

#include "main.h"
#include "bl_jump.h"
#include "flash_layout.h"


typedef void (*pFunction)(void);
void JumpToApplication(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;
    pFunction appEntry;

    /* Read application stack pointer */
    appStack = *(volatile uint32_t*)APP_CODE_ADDR;

    /* Validate stack pointer */
    if ((appStack & 0x2FF00000) != 0x20000000)
    {
        return; // no valid app
    }

    uint64_t fw_id =  *(uint64_t*)(APP_CODE_ADDR + FW_IDENTITY_OFFSET);

    if (fw_id != FW_IDENTITY_VALUE)
    {
        /* This firmware is NOT built for this product */
    	return;   // refuse to boot
    }
    /* Read reset handler */
    appResetHandler = *(volatile uint32_t*)(APP_CODE_ADDR + 4);
    appEntry = (pFunction)appResetHandler;

    __disable_irq();

    /* Stop SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Disable all NVIC interrupts */
    for (uint32_t i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* Reset clock tree to HSI (reset-like state) */
    HAL_RCC_DeInit();
     HAL_DeInit();


    /* Set vector table */
    SCB->VTOR = APP_CODE_ADDR;

    /* Set MSP */
    __set_MSP(appStack);

    /* Jump to application */
    appEntry();
}

