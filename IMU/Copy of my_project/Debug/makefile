################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/subdir.mk
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
all: my_project.elf

dependents:
	-cd /home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Debug && $(MAKE) all

# Tool invocations
my_project.elf: $(OBJS) $(USER_OBJS) /home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Debug/libstm32f072b-disco_stdperiph_lib.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -L"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Debug" -T"/home/chrisg/workspace_stm/my_project/LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm -o "my_project.elf" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "my_project.elf" "my_project.bin"
	arm-none-eabi-size "my_project.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Debug/libstm32f072b-disco_stdperiph_lib.a:

-include ../makefile.targets
