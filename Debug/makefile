################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := cs-rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include usart/subdir.mk
-include src/subdir.mk
-include sd_card/fatfs/subdir.mk
-include rc5/subdir.mk
-include enc28j60/subdir.mk
-include Setup/subdir.mk
-include SERVER/tcpip/subdir.mk
-include SERVER/subdir.mk
-include RTC/subdir.mk
-include Library/STM32F10x_StdPeriph_Driver/src/subdir.mk
-include Library/CMSIS/CM3/DeviceSupport/ST/STM32F10x/subdir.mk
-include Library/CMSIS/CM3/CoreSupport/subdir.mk
-include CMD/subdir.mk
-include 1wire/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Mini_server.hex \

SECONDARY_LIST += \
Mini_server.lst \

SECONDARY_SIZE += \
Mini_server.siz \


# All Target
all: Mini_server.elf secondary-outputs

# Tool invocations
Mini_server.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Windows GCC C++ Linker'
	arm-none-eabi-g++ -T"F:\Moje\STM32\workspace\MiniServerEU\Setup\stm32_flash.ld" -Xlinker --gc-sections -Wl,-Map,Mini_server.map -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2 -o "Mini_server.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Mini_server.hex: Mini_server.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex Mini_server.elf  "Mini_server.hex"
	@echo 'Finished building: $@'
	@echo ' '

Mini_server.lst: Mini_server.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Listing'
	arm-none-eabi-objdump -h -S Mini_server.elf > "Mini_server.lst"
	@echo 'Finished building: $@'
	@echo ' '

Mini_server.siz: Mini_server.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Print Size'
	arm-none-eabi-size  --format=berkeley -x -t Mini_server.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(CXX_DEPS)$(S_UPPER_DEPS)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS) Mini_server.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
