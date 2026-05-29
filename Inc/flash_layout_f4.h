/*
 * flash_layout_f4.h
 *
 *  Created on: May 28, 2026
 *      Author: Sriram
 */

#ifndef INC_FLASH_LAYOUT_F4_H_
#define INC_FLASH_LAYOUT_F4_H_

#define FLASH_PAGE_SIZE          0x4000U     // 16KB sector-based erase unit (STM32F4)
#define FLASH_SECTOR_SIZE        FLASH_PAGE_SIZE
#define FLASH_ERASE_BLOCK_SIZE   FLASH_PAGE_SIZE

/* STM32F407VGTX geometry: 1 MB single-bank flash with mixed sector sizes.
 * Keep the partition boundaries aligned to sector boundaries so whole-sector erase works. */
#define FLASH_BASE_ADDR          0x08000000U
#define FLASH_SIZE               (1024U * 1024U)
#define FLASH_END_ADDR           (FLASH_BASE_ADDR + FLASH_SIZE - 1U)
#define FLASH_BANK_SIZE          (128U * 1024U)

/* Bootloader region (16 kB) */
#define BL_START_ADDR            FLASH_BASE_ADDR
#define BL_SIZE                  0x4000U

/* =========== DUAL PARTITION LAYOUT (118 kB each) =========== */
#define PART_A_START_ADDR        0x08020000U
#define PART_A_SIZE              0x20000U
#define PART_A_END_ADDR          (PART_A_START_ADDR + PART_A_SIZE - 1U)

/* Partition B starts on the next 128 KB sector boundary so erasing A does not wipe B. */
#define PART_B_START_ADDR        0x08040000U
#define PART_B_SIZE              0x20000U
#define PART_B_END_ADDR          (PART_B_START_ADDR + PART_B_SIZE - 1U)

/* OTA header remains 2KB metadata block. Sector erase is handled by backend. */
#define OTA_HDR_SIZE             (16U * 1024U)
#define OTA_HDR_ADDR             0x08004000U

#define PART_A_APP_START         PART_A_START_ADDR
#define PART_B_APP_START         PART_B_START_ADDR

#define APP_CODE_ADDR            PART_A_APP_START
#define APP_START_ADDR           PART_A_START_ADDR

#define OTA_HDR_MAGIC            0xDEADBEEF

#define OTA_STATE_EMPTY          0xFFFFFFFF
#define OTA_STATE_UPDATING       0x55555555
#define OTA_STATE_VALID          0xAAAAAAAA

#define PARTITION_A              0
#define PARTITION_B              1

#define FW_IDENTITY_OFFSET       0x188
#define FW_IDENTITY_VALUE        0x4E4553484C495645ULL
#define BOOTLOADER_VERSION       0x020001

#define SP_MASK 0x2FF00000
#endif /* INC_FLASH_LAYOUT_F4_H_ */
