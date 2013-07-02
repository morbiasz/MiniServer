################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sd_card/fatfs/fattime.c \
../sd_card/fatfs/ff.c \
../sd_card/fatfs/sd_spi_stm32.c 

OBJS += \
./sd_card/fatfs/fattime.o \
./sd_card/fatfs/ff.o \
./sd_card/fatfs/sd_spi_stm32.o 

C_DEPS += \
./sd_card/fatfs/fattime.d \
./sd_card/fatfs/ff.d \
./sd_card/fatfs/sd_spi_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
sd_card/fatfs/%.o: ../sd_card/fatfs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -I"F:\Moje\STM32\workspace\MiniServerEU\Library\CMSIS\CM3\CoreSupport" -I"F:\Moje\STM32\workspace\MiniServerEU\sd_card\fatfs" -I"F:\Moje\STM32\workspace\MiniServerEU\1wire" -I"F:\Moje\STM32\workspace\MiniServerEU\enc28j60" -I"F:\Moje\STM32\workspace\MiniServerEU\rc5" -I"F:\Moje\STM32\workspace\MiniServerEU\sd_card" -I"F:\Moje\STM32\workspace\MiniServerEU\usart" -I"F:\Moje\STM32\workspace\MiniServerEU\src" -I"F:\Moje\STM32\workspace\MiniServerEU" -I"F:\Moje\STM32\workspace\MiniServerEU\Library\CMSIS\CM3\DeviceSupport\ST\STM32F10x" -I"F:\Moje\STM32\workspace\MiniServerEU\Library\STM32F10x_StdPeriph_Driver\inc" -O1 -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


