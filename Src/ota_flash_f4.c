/*
 * ota_flash_f4.c
 *
 *  Created on: May 28, 2026
 *      Author: Sriram
 */

#ifdef FAMILY_F4

#include "ota_flash_platform.h"

#include "flash_layout.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash_ex.h"

#ifndef FLASH_BANK_1
#define FLASH_BANK_1 ((uint32_t)0U)
#endif

#ifndef FLASH_BANK_2
#define FLASH_BANK_2 ((uint32_t)1U)
#endif

const uint32_t ota_flash_erase_block_size = (16U * 1024U);

typedef struct
{
    uint32_t start;
    uint32_t end;
} flash_sector_region_t;

static const flash_sector_region_t flash_sector_regions[] =
{
    {0x08000000U, 0x08003FFFU},
    {0x08004000U, 0x08007FFFU},
    {0x08008000U, 0x0800BFFFU},
    {0x0800C000U, 0x0800FFFFU},
    {0x08010000U, 0x0801FFFFU},
    {0x08020000U, 0x0803FFFFU},
    {0x08040000U, 0x0805FFFFU},
    {0x08060000U, 0x0807FFFFU},
    {0x08080000U, 0x0809FFFFU},
    {0x080A0000U, 0x080BFFFFU},
    {0x080C0000U, 0x080DFFFFU},
    {0x080E0000U, 0x080FFFFFU},
};

static bool Flash_FindSectorForAddress(uint32_t addr, uint32_t *sector_index, uint32_t *sector_start, uint32_t *sector_end)
{
    for (uint32_t i = 0U; i < (sizeof(flash_sector_regions) / sizeof(flash_sector_regions[0])); i++)
    {
        if (addr >= flash_sector_regions[i].start && addr <= flash_sector_regions[i].end)
        {
            *sector_index = i;
            *sector_start = flash_sector_regions[i].start;
            *sector_end = flash_sector_regions[i].end + 1U;
            return true;
        }
    }

    return false;
}

bool OTA_Flash_Program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    if (data == NULL && len != 0U)
        return false;

    HAL_FLASH_Unlock();

    for (uint32_t i = 0U; i < len; i += 2U)
    {
        uint16_t hw;

        if (i + 1U < len)
        {
            hw = data[i] | ((uint16_t)data[i + 1U] << 8);
        }
        else
        {
            hw = data[i] | (0xFFU << 8);
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, hw) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return false;
        }

        addr += 2U;
    }

    HAL_FLASH_Lock();
    return true;
}

bool OTA_Flash_EraseRegion(uint32_t addr, uint32_t size)
{
    if (size == 0U)
        return true;

    if (addr < FLASH_BASE_ADDR || addr > FLASH_END_ADDR)
        return false;

    if (addr + size < addr)
        return false;

    if (addr + size > (FLASH_END_ADDR + 1U))
        return false;

    uint32_t region_start = addr;
    uint32_t region_end   = addr + size;

    HAL_FLASH_Unlock();

    while (region_start < region_end)
    {
        uint32_t sector_index = 0U;
        uint32_t sector_start = 0U;
        uint32_t sector_end   = 0U;

        if (!Flash_FindSectorForAddress(region_start, &sector_index, &sector_start, &sector_end))
        {
            HAL_FLASH_Lock();
            return false;
        }

        FLASH_EraseInitTypeDef erase = {0};
        uint32_t error = 0U;

        erase.TypeErase   = FLASH_TYPEERASE_SECTORS;
        erase.Banks       = FLASH_BANK_1;
        erase.Sector      = sector_index;
        erase.NbSectors   = 1U;
        erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

        HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase, &error);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return false;
        }

        region_start = sector_end;
    }

    HAL_FLASH_Lock();
    return true;
}

bool OTA_Flash_WriteHeader(const ota_header_t *hdr)
{
    if (hdr == NULL)
        return false;

    if (!OTA_Flash_EraseRegion(OTA_HDR_ADDR, ota_flash_erase_block_size))
        return false;

    return OTA_Flash_Program(OTA_HDR_ADDR, (const uint8_t *)hdr, sizeof(ota_header_t));
}

#endif
