/*
 * ota_flash_platform.h
 *
 *  Created on: May 28, 2026
 *      Author: Copilot
 */

#ifndef OTA_FLASH_PLATFORM_H
#define OTA_FLASH_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#include "bl_ota_header.h"

extern const uint32_t ota_flash_erase_block_size;

bool OTA_Flash_Program(uint32_t addr, const uint8_t *data, uint32_t len);
bool OTA_Flash_EraseRegion(uint32_t addr, uint32_t size);
bool OTA_Flash_WriteHeader(const ota_header_t *hdr);

#endif /* OTA_FLASH_PLATFORM_H */
