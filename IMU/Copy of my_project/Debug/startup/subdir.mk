################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f072.s 

OBJS += \
./startup/startup_stm32f072.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib" -I/opt/ros/indigo/include -I"/home/chrisg/workspace_stm/my_project/inc" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/StdPeriph_Driver/inc" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/CMSIS/core" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/CMSIS/device" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Utilities/STM32F072B-Discovery" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


