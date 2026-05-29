/*
 * bl_flash.h
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */

#ifndef BL_FLASH_H
#define BL_FLASH_H

#include "bl_ota_header.h"
#include <stdint.h>
#include <stdbool.h>

/* erase application region; returns false if the requested size would
   overlap the bootloader or exceed flash limits */
bool Flash_Erase_App(uint32_t size);
void Flash_Program(uint32_t addr, uint8_t *data, uint32_t len);
void Flash_Write_Header(ota_header_t *hdr);

/* Partition operations for dual A/B OTA scheme */
bool Flash_Erase_Partition(uint32_t partition);        /* PARTITION_A or PARTITION_B */
bool Flash_Copy_Partition(uint32_t src, uint32_t dst, uint32_t size);
bool Flash_Verify_Partition_CRC(uint32_t partition, uint32_t size, uint32_t expected_crc);

#endif

