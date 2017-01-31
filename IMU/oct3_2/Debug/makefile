################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/tm/subdir.mk
-include src/subdir.mk
-include Utilities/STM32F072B-Discovery/subdir.mk
-include Utilities/Components/stlm75/subdir.mk
-include Utilities/Components/st7735/subdir.mk
-include Utilities/Components/spfd5408/subdir.mk
-include Utilities/Components/l3gd20/subdir.mk
-include Utilities/Components/hx8347d/subdir.mk
-include HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: oct3_2.elf

# Tool invocations
oct3_2.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -T"/home/chrisg/workspace_stm/oct3_2/LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm -o "oct3_2.elf" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "oct3_2.elf" "oct3_2.bin"
	arm-none-eabi-size "oct3_2.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
