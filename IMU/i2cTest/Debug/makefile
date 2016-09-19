################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Src/subdir.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/subdir.mk
-include Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/subdir.mk
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
all: i2cTest.elf

# Tool invocations
i2cTest.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs -specs=rdimon.specs -lc -lrdimon -T"../STM32F072RBTx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm -o "i2cTest.elf" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "i2cTest.elf" "i2cTest.bin"
	arm-none-eabi-size "i2cTest.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
