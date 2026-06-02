################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../stm_bootloader/Src/bl_flash.c \
../stm_bootloader/Src/bl_jump.c \
../stm_bootloader/Src/bl_uart.c \
../stm_bootloader/Src/bl_update.c \
../stm_bootloader/Src/boot_req.c \
../stm_bootloader/Src/crc32.c \
../stm_bootloader/Src/ota.c \
../stm_bootloader/Src/ota_flash_f1.c \
../stm_bootloader/Src/ota_flash_f4.c 

OBJS += \
./stm_bootloader/Src/bl_flash.o \
./stm_bootloader/Src/bl_jump.o \
./stm_bootloader/Src/bl_uart.o \
./stm_bootloader/Src/bl_update.o \
./stm_bootloader/Src/boot_req.o \
./stm_bootloader/Src/crc32.o \
./stm_bootloader/Src/ota.o \
./stm_bootloader/Src/ota_flash_f1.o \
./stm_bootloader/Src/ota_flash_f4.o 

C_DEPS += \
./stm_bootloader/Src/bl_flash.d \
./stm_bootloader/Src/bl_jump.d \
./stm_bootloader/Src/bl_uart.d \
./stm_bootloader/Src/bl_update.d \
./stm_bootloader/Src/boot_req.d \
./stm_bootloader/Src/crc32.d \
./stm_bootloader/Src/ota.d \
./stm_bootloader/Src/ota_flash_f1.d \
./stm_bootloader/Src/ota_flash_f4.d 


# Each subdirectory must supply rules for building sources it contributes
stm_bootloader/Src/%.o stm_bootloader/Src/%.su stm_bootloader/Src/%.cyclo: ../stm_bootloader/Src/%.c stm_bootloader/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DFAMILY_F1 -DUSE_HAL_DRIVER -DSTM32F105xC -c -I../stm_bootloader/Inc -I../stm_bootloader/Src -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-stm_bootloader-2f-Src

clean-stm_bootloader-2f-Src:
	-$(RM) ./stm_bootloader/Src/bl_flash.cyclo ./stm_bootloader/Src/bl_flash.d ./stm_bootloader/Src/bl_flash.o ./stm_bootloader/Src/bl_flash.su ./stm_bootloader/Src/bl_jump.cyclo ./stm_bootloader/Src/bl_jump.d ./stm_bootloader/Src/bl_jump.o ./stm_bootloader/Src/bl_jump.su ./stm_bootloader/Src/bl_uart.cyclo ./stm_bootloader/Src/bl_uart.d ./stm_bootloader/Src/bl_uart.o ./stm_bootloader/Src/bl_uart.su ./stm_bootloader/Src/bl_update.cyclo ./stm_bootloader/Src/bl_update.d ./stm_bootloader/Src/bl_update.o ./stm_bootloader/Src/bl_update.su ./stm_bootloader/Src/boot_req.cyclo ./stm_bootloader/Src/boot_req.d ./stm_bootloader/Src/boot_req.o ./stm_bootloader/Src/boot_req.su ./stm_bootloader/Src/crc32.cyclo ./stm_bootloader/Src/crc32.d ./stm_bootloader/Src/crc32.o ./stm_bootloader/Src/crc32.su ./stm_bootloader/Src/ota.cyclo ./stm_bootloader/Src/ota.d ./stm_bootloader/Src/ota.o ./stm_bootloader/Src/ota.su ./stm_bootloader/Src/ota_flash_f1.cyclo ./stm_bootloader/Src/ota_flash_f1.d ./stm_bootloader/Src/ota_flash_f1.o ./stm_bootloader/Src/ota_flash_f1.su ./stm_bootloader/Src/ota_flash_f4.cyclo ./stm_bootloader/Src/ota_flash_f4.d ./stm_bootloader/Src/ota_flash_f4.o ./stm_bootloader/Src/ota_flash_f4.su

.PHONY: clean-stm_bootloader-2f-Src

