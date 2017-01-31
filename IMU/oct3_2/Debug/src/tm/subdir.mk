################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/tm/tm_stm32_delay.c \
../src/tm/tm_stm32_gpio.c \
../src/tm/tm_stm32_i2c.c \
../src/tm/tm_stm32_mpu6050.c 

OBJS += \
./src/tm/tm_stm32_delay.o \
./src/tm/tm_stm32_gpio.o \
./src/tm/tm_stm32_i2c.o \
./src/tm/tm_stm32_mpu6050.o 

C_DEPS += \
./src/tm/tm_stm32_delay.d \
./src/tm/tm_stm32_gpio.d \
./src/tm/tm_stm32_i2c.d \
./src/tm/tm_stm32_mpu6050.d 


# Each subdirectory must supply rules for building sources it contributes
src/tm/%.o: ../src/tm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F072xB -I"/home/chrisg/workspace_stm/oct3_2/inc" -I"/home/chrisg/workspace_stm/oct3_2/CMSIS/core" -I"/home/chrisg/workspace_stm/oct3_2/CMSIS/device" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/STM32F072B-Discovery" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/spfd5408" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/Common" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/l3gd20" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/hx8347d" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/st7735" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/stlm75" -I"/home/chrisg/workspace_stm/oct3_2/HAL_Driver/Inc" -I"/home/chrisg/workspace_stm/oct3_2/HAL_Driver/Inc/Legacy" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


