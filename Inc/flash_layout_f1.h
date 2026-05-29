/*
 * flash_layout_f1.h
 *
 *  Created on: May 28, 2026
 *      Author: Copilot
 */

#ifndef INC_FLASH_LAYOUT_F1_H_
#define INC_FLASH_LAYOUT_F1_H_

#define FLASH_PAGE_SIZE          0x800U      // 2KB (STM32F1)
#define FLASH_SECTOR_SIZE        FLASH_PAGE_SIZE
#define FLASH_ERASE_BLOCK_SIZE   FLASH_PAGE_SIZE

/* full flash geometry (STM32F105RCTX = 256 kB) */
#define FLASH_BASE_ADDR          0x08000000U
#define FLASH_SIZE               (256U * 1024U)
#define FLASH_END_ADDR           (FLASH_BASE_ADDR + FLASH_SIZE - 1U)

/* Bootloader (16 kB) */
#define BL_START_ADDR            FLASH_BASE_ADDR
#define BL_SIZE                  0x4000U      /* 16 kB */

/* =========== DUAL PARTITION LAYOUT (118 kB each) =========== */
/* Partition A: 0x08004000 - 0x08021FFF (118 kB) */
#define PART_A_START_ADDR        0x08004000U
#define PART_A_SIZE              0x1D800U     /* 118 kB */
#define PART_A_END_ADDR          (PART_A_START_ADDR + PART_A_SIZE - 1U)

/* Partition B: 0x08022000 - 0x0803F7FF (118 kB) */
#define PART_B_START_ADDR        0x08022000U
#define PART_B_SIZE              0x1D800U     /* 118 kB */
#define PART_B_END_ADDR          (PART_B_START_ADDR + PART_B_SIZE - 1U)

/* OTA Header location (at start of Partition A) */
#define OTA_HDR_SIZE             (2U * 1024U)
#define OTA_HDR_ADDR             PART_A_START_ADDR

/* Application code starts after OTA header in Partition A */
#define PART_A_APP_START         (PART_A_START_ADDR + OTA_HDR_SIZE)    /* 0x08004800 */

/* Application code in Partition B (no header, just code) */
#define PART_B_APP_START         PART_B_START_ADDR   /* 0x08022000 */

/* Legacy compatibility: APP_CODE_ADDR points to Partition A app location */
#define APP_CODE_ADDR            PART_A_APP_START
#define APP_START_ADDR           PART_A_START_ADDR

/* OTA header states */
#define OTA_HDR_MAGIC            0xDEADBEEF

#define OTA_STATE_EMPTY          0xFFFFFFFF
#define OTA_STATE_UPDATING       0x55555555
#define OTA_STATE_VALID          0xAAAAAAAA

/* Partition selection flags */
#define PARTITION_A              0
#define PARTITION_B              1

#define FW_IDENTITY_OFFSET       0x1E8
#define FW_IDENTITY_VALUE        0x4E4553484C495645ULL   // "NESHLIVE"
#define BOOTLOADER_VERSION       0x020001

#define SP_MASK 0x2FFE0000
#endif /* INC_FLASH_LAYOUT_F1_H_ */
