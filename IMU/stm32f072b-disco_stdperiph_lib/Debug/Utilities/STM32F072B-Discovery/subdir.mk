################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/STM32F072B-Discovery/stm32f072b_discovery.c \
../Utilities/STM32F072B-Discovery/stm32f072b_discovery_i2c_ee.c \
../Utilities/STM32F072B-Discovery/stm32f072b_discovery_l3gd20.c 

OBJS += \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery.o \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_i2c_ee.o \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_l3gd20.o 

C_DEPS += \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery.d \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_i2c_ee.d \
./Utilities/STM32F072B-Discovery/stm32f072b_discovery_l3gd20.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/STM32F072B-Discovery/%.o: ../Utilities/STM32F072B-Discovery/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DUSE_STDPERIPH_DRIVER -DSTM32F072 -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/StdPeriph_Driver/inc" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/CMSIS/core" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/CMSIS/device" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Utilities/STM32F072B-Discovery" -I/opt/ros/indigo/include -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


