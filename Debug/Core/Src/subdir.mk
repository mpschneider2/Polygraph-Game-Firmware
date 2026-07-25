################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/U8g2_for_Adafruit_GFX.c \
../Core/Src/adc_driver.c \
../Core/Src/app_states.c \
../Core/Src/buttons.c \
../Core/Src/data_processing.c \
../Core/Src/filters.c \
../Core/Src/gc9a01a.c \
../Core/Src/gsr.c \
../Core/Src/main.c \
../Core/Src/ppg_processor.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/u8g2_fonts.c 

OBJS += \
./Core/Src/U8g2_for_Adafruit_GFX.o \
./Core/Src/adc_driver.o \
./Core/Src/app_states.o \
./Core/Src/buttons.o \
./Core/Src/data_processing.o \
./Core/Src/filters.o \
./Core/Src/gc9a01a.o \
./Core/Src/gsr.o \
./Core/Src/main.o \
./Core/Src/ppg_processor.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/u8g2_fonts.o 

C_DEPS += \
./Core/Src/U8g2_for_Adafruit_GFX.d \
./Core/Src/adc_driver.d \
./Core/Src/app_states.d \
./Core/Src/buttons.d \
./Core/Src/data_processing.d \
./Core/Src/filters.d \
./Core/Src/gc9a01a.d \
./Core/Src/gsr.d \
./Core/Src/main.d \
./Core/Src/ppg_processor.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/u8g2_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../FATFS/Target -I../FATFS/App -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/U8g2_for_Adafruit_GFX.cyclo ./Core/Src/U8g2_for_Adafruit_GFX.d ./Core/Src/U8g2_for_Adafruit_GFX.o ./Core/Src/U8g2_for_Adafruit_GFX.su ./Core/Src/adc_driver.cyclo ./Core/Src/adc_driver.d ./Core/Src/adc_driver.o ./Core/Src/adc_driver.su ./Core/Src/app_states.cyclo ./Core/Src/app_states.d ./Core/Src/app_states.o ./Core/Src/app_states.su ./Core/Src/buttons.cyclo ./Core/Src/buttons.d ./Core/Src/buttons.o ./Core/Src/buttons.su ./Core/Src/data_processing.cyclo ./Core/Src/data_processing.d ./Core/Src/data_processing.o ./Core/Src/data_processing.su ./Core/Src/filters.cyclo ./Core/Src/filters.d ./Core/Src/filters.o ./Core/Src/filters.su ./Core/Src/gc9a01a.cyclo ./Core/Src/gc9a01a.d ./Core/Src/gc9a01a.o ./Core/Src/gc9a01a.su ./Core/Src/gsr.cyclo ./Core/Src/gsr.d ./Core/Src/gsr.o ./Core/Src/gsr.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/ppg_processor.cyclo ./Core/Src/ppg_processor.d ./Core/Src/ppg_processor.o ./Core/Src/ppg_processor.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/u8g2_fonts.cyclo ./Core/Src/u8g2_fonts.d ./Core/Src/u8g2_fonts.o ./Core/Src/u8g2_fonts.su

.PHONY: clean-Core-2f-Src

