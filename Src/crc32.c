/*
 * crc32.c
 *
 *  Created on: Jan 7, 2026
 *      Author: Sriram
 */

#include "crc32.h"

uint32_t crc32(uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
    }

    return ~crc;
}

