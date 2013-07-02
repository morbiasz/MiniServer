################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../enc28j60/enc28j60.c \
../enc28j60/spi.c 

OBJS += \
./enc28j60/enc28j60.o \
./enc28j60/spi.o 

C_DEPS += \
./enc28j60/enc28j60.d \
./enc28j60/spi.d 


# Each subdirectory must supply rules for building sources it contributes
enc28j60/%.o: ../enc28j60/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -I"F:\Moje\STM32\workspace\MiniServerEU\Library\CMSIS\CM3\CoreSupport" -I"F:\Moje\STM32\workspace\MiniServerEU\sd_card\fatfs" -I"F:\Moje\STM32\workspace\MiniServerEU\1wire" -I"F:\Moje\STM32\workspace\MiniServerEU\enc28j60" -I"F:\Moje\STM32\workspace\MiniServerEU\rc5" -I"F:\Moje\STM32\workspace\MiniServerEU\sd_card" -I"F:\Moje\STM32\workspace\MiniServerEU\usart" -I"F:\Moje\STM32\workspace\MiniServerEU\src" -I"F:\Moje\STM32\workspace\MiniServerEU" -I"F:\Moje\STM32\workspace\MiniServerEU\Library\CMSIS\CM3\DeviceSupport\ST\STM32F10x" -I"F:\Moje\STM32\workspace\MiniServerEU\Library\STM32F10x_StdPeriph_Driver\inc" -O1 -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


