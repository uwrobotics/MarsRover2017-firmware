################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: libstm32f072b-disco_hal_lib.a

# Tool invocations
libstm32f072b-disco_hal_lib.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Archiver'
	arm-none-eabi-ar -r  "libstm32f072b-disco_hal_lib.a" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
