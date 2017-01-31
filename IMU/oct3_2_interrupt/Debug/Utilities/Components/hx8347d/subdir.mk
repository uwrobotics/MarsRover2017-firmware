################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Components/hx8347d/hx8347d.c 

OBJS += \
./Utilities/Components/hx8347d/hx8347d.o 

C_DEPS += \
./Utilities/Components/hx8347d/hx8347d.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Components/hx8347d/%.o: ../Utilities/Components/hx8347d/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F072xB -I"/home/chrisg/workspace_stm/oct3_2_interrupt/inc" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/CMSIS/core" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/CMSIS/device" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/STM32F072B-Discovery" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/Components/spfd5408" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/Components/Common" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/Components/l3gd20" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/Components/hx8347d" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/Components/st7735" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/Utilities/Components/stlm75" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/HAL_Driver/Inc" -I"/home/chrisg/workspace_stm/oct3_2_interrupt/HAL_Driver/Inc/Legacy" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


