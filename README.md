# Bootloader usage

This project exposes the OTA boot entry point through `ota.h`.

## Include and call the boot routine

Include the header in your application and pass the UART handle that the bootloader should use:

```c
#include "ota.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    /* Initialize the UART used for OTA / bootloader communication */
    MX_USART1_UART_Init();

    /* Start the bootloader on the selected UART handle */
    boot(&huart1);

    /* If boot() returns, continue with normal application flow */
    while (1)
    {
    }
}
```

Replace `&huart1` with the correct UART handle for your board (for example `&huart2`, `&huart3`, etc.).

## Build configuration

The flash layout implementation is selected by the STM32 family macro:

- STM32F1 family: define `FAMILY_F1`
- STM32F4 family: define `FAMILY_F4`

Example compiler flags:

```text
# STM32F1 build
CFLAGS += -DFAMILY_F1

# STM32F4 build
CFLAGS += -DFAMILY_F4
```

These macros are required so the correct family-specific flash layout code is included.

## Flash layout

The bootloader uses family-specific flash geometry. The values below match the layouts defined in the family headers.

### STM32F1 family (`FAMILY_F1`)

- Flash erase unit: `2 KB` page / sector (`FLASH_PAGE_SIZE = 0x800`)
- Bootloader region: `0x08000000` to `0x08003FFF` (16 KB)
- Partition A: `0x08004000` to `0x08021FFF` (118 KB)
- Partition B: `0x08022000` to `0x0803F7FF` (118 KB)
- OTA header starts at `0x08004000` with `2 KB` metadata space
- Application start in Partition A: `0x08004800`

### STM32F4 family (`FAMILY_F4`)

- Flash erase unit: `16 KB` sector (`FLASH_PAGE_SIZE = 0x4000`)
- Bootloader region: `0x08000000` to `0x08003FFF` (16 KB)
- Partition A: `0x08020000` to `0x0803FFFF` (128 KB)
- Partition B: `0x08040000` to `0x0805FFFF` (128 KB)
- OTA header address: `0x08004000` with `16 KB` metadata region
- Application start in Partition A: `0x08020000`

## Notes

- `ota.h` declares `boot(void *uart)`.
- Pass the UART handle directly; the function expects a pointer to the active UART instance.
- Use the matching family define for your MCU so the correct partition/layout code is compiled.
