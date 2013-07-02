################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SERVER/dhcp_client.c \
../SERVER/dnslkup.c \
../SERVER/serverWWW.c \
../SERVER/timeconversions.c \
../SERVER/websrv_help_functions.c 

OBJS += \
./SERVER/dhcp_client.o \
./SERVER/dnslkup.o \
./SERVER/serverWWW.o \
./SERVER/timeconversions.o \
./SERVER/websrv_help_functions.o 

C_DEPS += \
./SERVER/dhcp_client.d \
./SERVER/dnslkup.d \
./SERVER/serverWWW.d \
./SERVER/timeconversions.d \
./SERVER/websrv_help_functions.d 


# Each subdirectory must supply rules for building sources it contributes
SERVER/%.o: ../SERVER/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -I"F:\Moje\STM32\workspace\MiniServerEU\Library\CMSIS\CM3\CoreSupport" -I"F:\Moje\STM32\workspace\MiniServerEU\sd_card\fatfs" -I"F:\Moje\STM32\workspace\MiniServerEU\1wire" -I"F:\Moje\STM32\workspace\MiniServerEU\enc28j60" -I"F:\Moje\STM32\workspace\MiniServerEU\rc5" -I"F:\Moje\STM32\workspace\MiniServerEU\sd_card" -I"F:\Moje\STM32\workspace\MiniServerEU\usart" -I"F:\Moje\STM32\workspace\MiniServerEU\src" -I"F:\Moje\STM32\workspace\MiniServerEU" -I"F:\Moje\STM32\workspace\MiniServerEU\Library\CMSIS\CM3\DeviceSupport\ST\STM32F10x" -I"F:\Moje\STM32\workspace\MiniServerEU\Library\STM32F10x_StdPeriph_Driver\inc" -O1 -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


