################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/STM32F072B-Discovery/stm32f072b_discovery.c \
../Utilities/STM32F072B-Discovery/stm32f072b_discovery_eeprom.c \
../Utilities/STM32F072B-Discovery/stm32f072b_discovery_gyroscope.c 

OBJS += \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery.o \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_eeprom.o \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_gyroscope.o 

C_DEPS += \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery.d \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_eeprom.d \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_gyroscope.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/STM32F072B-Discovery/%.o: ../Utilities/STM32F072B-Discovery/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DUSE_HAL_DRIVER -DSTM32F072xB -I/opt/ros/indigo/include -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/CMSIS/core" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/CMSIS/device" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/STM32F072B-Discovery" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/Components/spfd5408" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/Components/Common" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/Components/l3gd20" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/Components/hx8347d" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/Components/st7735" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/Utilities/Components/stlm75" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/HAL_Driver/Inc" -I"/home/chrisg/workspace_stm/stm32f072b-disco_hal_lib/HAL_Driver/Inc/Legacy" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


