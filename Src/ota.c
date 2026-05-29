/*
 * ota.c
 *
 *  Created on: May 28, 2026
 *      Author: Sriram
 */

#include "ota.h"
#include "bl_uart.h"
#include "bl_update.h"
#include "bl_jump.h"
#include "bl_flash.h"
#include "boot_req.h"
#include "flash_layout.h"
#include "bl_ota_header.h"
#include "crc32.h"
#include "main.h"

void boot (void *uart)
{
	BL_UART_Init(uart);

	uint8_t  resp;
		  /* If magic is valid but state is UPDATING, it means update was interrupted.
	     Try to boot partition A anyway if it's still intact. */

	  if (OTA_HDR->magic != OTA_HDR_MAGIC)
	  {
	      /* Header is completely corrupted - try recovery from Partition B first */
	      resp = 0xE1;
	      HAL_UART_Transmit(BL_UART_GetHandle(), &resp, 1, 100);

	      /* Validate basic vector table at PART_B_APP_START */
	      uint32_t b_sp = *(volatile uint32_t*)PART_B_APP_START;
	      if ((b_sp & SP_MASK) == 0x20000000)
	      {
	          /* Check firmware identity in partition B */
	          uint64_t b_fw_id = *(uint64_t*)(PART_B_APP_START + FW_IDENTITY_OFFSET);
	          if (b_fw_id == FW_IDENTITY_VALUE)
	          {
	              /* Heuristic: determine actual image size in partition B by scanning
	                 for trailing 0xFF (erased flash). */
	              int32_t scan_addr = (int32_t)(PART_B_START_ADDR + PART_B_SIZE - 1);
	              while (scan_addr >= (int32_t)PART_B_APP_START && (*(volatile uint8_t*)scan_addr) == 0xFF)
	                  scan_addr--;

	              if (scan_addr >= (int32_t)PART_B_APP_START)
	              {
	                  uint32_t image_size = (uint32_t)(scan_addr - PART_B_APP_START + 1);

	                  if (image_size > 0)
	                  {
	                      uint32_t calc_crc = crc32((uint8_t*)PART_B_APP_START, image_size);

	                      /* Erase partition A and copy B->A */
	                      if (Flash_Erase_Partition(PARTITION_A) && Flash_Copy_Partition(PART_B_APP_START, PART_A_APP_START, image_size))
	                      {
	                          /* Write a new valid header describing the recovered image */
	                          ota_header_t newhdr;
	                          newhdr.magic = OTA_HDR_MAGIC;
	                          newhdr.image_size = image_size;
	                          newhdr.image_crc  = calc_crc;
	                          newhdr.state = OTA_STATE_VALID;
	                          newhdr.bootloader_version = BOOTLOADER_VERSION;
	                          newhdr.partition_select = PARTITION_A; /* now running A */
	                          newhdr.partition_b_crc = calc_crc;

	                          Flash_Write_Header(&newhdr);

	                          /* Proceed to normal boot flow (header now valid)
	                             fall through */
	                          JumpToApplication();
	                      }
	                  }
	              }
	          }
	      }


	      /* If any recovery step failed, enter OTA request loop */
	      OTA_RequestLoop();     // does not return
	  }

	  /* If state is UPDATING (interrupted update), we'll validate partition A below
	     and boot it if good. Only force OTA recovery if partition A fails validation. */
	  uint8_t header_state_valid = (OTA_HDR->state == OTA_STATE_VALID);
	  if (!header_state_valid && OTA_HDR->state != OTA_STATE_UPDATING)
	  {
	      /* Header state is invalid (not VALID or UPDATING) */
	      resp = 0xE1;
	      HAL_UART_Transmit(BL_UART_GetHandle(), &resp, 1, 100);
	      OTA_RequestLoop();     // does not return
	  }

	  /* ===================================================== */
	  /* 🟡 STEP B: APP REQUESTED OTA (RAM FLAG)                */
	  /* ===================================================== */

	  if (ota_request_flag == OTA_REQUEST_MAGIC)
	  {
	      ota_request_flag = 0;
		  resp = 0xE2;
		  HAL_UART_Transmit(BL_UART_GetHandle(), &resp, 1, 100);
	      OTA_RequestLoop();     // does not return
	  }

	  /* ===================================================== */
	  /* 🟢 STEP C: BASIC VALIDITY CHECK (STACK POINTER)        */
	  /* PARTITION A runs normally                              */
	  /* ===================================================== */

	  uint32_t sp = *(volatile uint32_t*)PART_A_APP_START;
	  if ((sp & SP_MASK) != 0x20000000)
	  {
		  resp = 0xE3;
		  HAL_UART_Transmit(BL_UART_GetHandle(), &resp, 1, 100);
	      OTA_RequestLoop();     // does not return
	  }

	  /* ===================================================== */
	  /* 🔵 STEP D: VERIFY FLASH CONTENT WITH STORED CRC        */
	  /* Verify partition A integrity                           */
	  /* ===================================================== */

	  /* Recalculate CRC of application currently in partition A */
	  uint32_t calc_crc = crc32((uint8_t*)PART_A_APP_START, OTA_HDR->image_size);

	  /* If firmware was changed / corrupted / wrong bin flashed */
	  if (calc_crc != OTA_HDR->image_crc)
	  {
	      /* Firmware no longer matches what OTA installed */
		  resp = 0xE4;
		  HAL_UART_Transmit(BL_UART_GetHandle(), &resp, 1, 100);

	      /* If header state is UPDATING (update was interrupted), but CRC is bad,
	         it means something went wrong. Force recovery. */
	      OTA_RequestLoop();     // force recovery update
	  }

	  /* If header state was UPDATING but CRC is good, mark it as VALID now */
	  if (!header_state_valid)
	  {
	      /* State was UPDATING but CRC passed - this is a recovered interrupted update.
	         Update the header to VALID. */
	      ota_header_t recovery_hdr = *OTA_HDR;
	      recovery_hdr.state = OTA_STATE_VALID;
	      Flash_Write_Header(&recovery_hdr);
	  }

	  /* ===================================================== */
	  /* 🟣 STEP E: VERIFY FIRMWARE IDENTITY (ANTI-WRONG BIN)   */
	  /* ===================================================== */

	  uint64_t fw_id =  *(uint64_t*)(PART_A_APP_START + FW_IDENTITY_OFFSET);

	  if (fw_id != FW_IDENTITY_VALUE)
	  {
	      /* This firmware is NOT built for this product */
		  resp = 0xE5;
		  HAL_UART_Transmit(BL_UART_GetHandle(), &resp, 1, 100);
	      OTA_RequestLoop();   // refuse to boot
	  }

	  /* ===================================================== */
	  /* ✅ STEP F: BOOT APPLICATION                            */
	  /* ===================================================== */

	//  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	  JumpToApplication();


	}
