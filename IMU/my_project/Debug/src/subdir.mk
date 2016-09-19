################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/i2c_master.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

OBJS += \
./src/i2c_master.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/i2c_master.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F072 -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib" -I/opt/ros/indigo/include -I"/home/chrisg/workspace_stm/my_project/inc" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/StdPeriph_Driver/inc" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/CMSIS/core" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/CMSIS/device" -I"/home/chrisg/workspace_stm/stm32f072b-disco_stdperiph_lib/Utilities/STM32F072B-Discovery" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


