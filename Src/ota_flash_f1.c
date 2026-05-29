/*
 * ota_flash_f1.c
 *
 *  Created on: May 28, 2026
 *      Author: Sriram
 */
#ifdef FAMILY_F1
#include "ota_flash_platform.h"

#include "flash_layout.h"
#include "stm32f1xx_hal.h"

const uint32_t ota_flash_erase_block_size = 0x800U;

static uint32_t AlignDown(uint32_t value, uint32_t alignment)
{
    return value & ~(alignment - 1U);
}

static uint32_t AlignUp(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1U) & ~(alignment - 1U);
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

    uint32_t erase_start = AlignDown(addr, ota_flash_erase_block_size);
    uint32_t erase_end   = AlignUp(addr + size, ota_flash_erase_block_size);

    if (erase_end < erase_start)
        return false;

    uint32_t nb_pages = (erase_end - erase_start) / ota_flash_erase_block_size;

    FLASH_EraseInitTypeDef erase = {0};
    uint32_t error = 0U;

    erase.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = erase_start;
    erase.NbPages     = nb_pages;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase, &error);
    HAL_FLASH_Lock();

    return (status == HAL_OK);
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
