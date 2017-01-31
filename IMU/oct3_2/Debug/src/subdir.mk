################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/stm32f0xx_it.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

OBJS += \
./src/main.o \
./src/stm32f0xx_it.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/main.d \
./src/stm32f0xx_it.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F072xB -I"/home/chrisg/workspace_stm/oct3_2/inc" -I"/home/chrisg/workspace_stm/oct3_2/CMSIS/core" -I"/home/chrisg/workspace_stm/oct3_2/CMSIS/device" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/STM32F072B-Discovery" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/spfd5408" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/Common" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/l3gd20" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/hx8347d" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/st7735" -I"/home/chrisg/workspace_stm/oct3_2/Utilities/Components/stlm75" -I"/home/chrisg/workspace_stm/oct3_2/HAL_Driver/Inc" -I"/home/chrisg/workspace_stm/oct3_2/HAL_Driver/Inc/Legacy" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


