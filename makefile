#General makefile for UWRT Mars Rover 2017

#these files have to be on the path
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size

APP_PATH = $(BASE_PATH)/app/$(APP_FOLDER)
BUILD_PATH = $(APP_PATH)/build
PROJ_PATH = $(BUILD_PATH)/$(PROJECT)
LIB_PATH = $(BASE_PATH)/lib

#os dependent code
ifeq ($(OS),Windows_NT)
REMOVE_PROGRAM = del /q
REMOVE_FOLDER = rmdir /s/q
GCC_INC = -IC:/Program\ Files\ (x86)/GNU\ Tools\ ARM\ Embedded/4.9\ 2015q3/arm-none-eabi/include\

else
REMOVE_PROGRAM = rm -rf
REMOVE_FOLDER = 
GCC_INC = 
endif

HAL = $(LIB_PATH)/stm32f072b-disco_hal_lib
# Location of the Libraries folder from the STM32F0xx Standard Peripheral Library
STD_PERIPH_LIB=$(HAL)/Debug

APP_SRC = $(wildcard $(APP_PATH)/src/*.c) 
LIB_SRC = $(wildcard $(LIB_PATH)/*/src/*.c)

APP_INC = -I$(APP_PATH)/inc 
LIB_INC = $(addprefix -I,$(wildcard $(LIB_PATH)/*/inc))

HAL_INC = -I$(HAL)/HAL_Driver/Inc -I$(HAL)/CMSIS/core -I$(HAL)/CMSIS/device

# Location of the linker scripts
LDSCRIPT_INC=$(HAL)

###################################################

CFLAGS  = -Wall -g -Os -std=gnu99
CFLAGS += -mlittle-endian -mcpu=cortex-m0  -march=armv6-m -mthumb
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wl,--gc-sections -Wl,-Map=$(PROJECT).map
CFLAGS += -u _printf_float -u _scanf_float

LFLAGS = $(CFLAGS) -lm

###################################################

vpath %.c src
vpath %.a $(STD_PERIPH_LIB)

ROOT=$(shell pwd)

# add startup file to build
SRCS = $(APP_SRC) $(LIB_SRC)
SRCASM = $(HAL)/startup/startup_stm32f072xb.S
INC = $(APP_INC) $(LIB_INC) $(HAL_INC)

# need if you want to build with -DUSE_CMSIS 
#SRCS += stm32f0_discovery.c
#SRCS += stm32f0_discovery.c stm32f0xx_it.c

OBJS = $(SRCS:.c=.o) $(SRCASM:.S=.o)
DEFINES = -DSTM32F0 -DSTM32F072B_DISCO -DSTM32F072RBTx -DSTM32 -DUSE_HAL_DRIVER -DSTM32F072xB


#os dependent code
ifeq ($(OS),Windows_NT)
CLEAN_PATH  = "$(APP_PATH)\*.map" $(addsuffix ", $(addprefix ", $(subst /,\,$(OBJS))))
CLEAN_FOLDER_PATH = "$(BUILD_PATH)"
else
CLEAN_PATH = $(BUILD_PATH) $(APP_PATH)/*.map $(OBJS)
CLEAN_FOLDER_PATH = 
endif


###################################################

.PHONY: proj clean

all: proj

proj: 	$(PROJ_PATH).elf

$(PROJ_PATH).elf: $(OBJS)
	@echo Building Project...
	@-mkdir build
	@$(CC) $(LFLAGS) $^ -L$(STD_PERIPH_LIB) -lstm32f072b-disco_hal_lib -L$(LDSCRIPT_INC) -TLinkerScript.ld -lm -lc -lg -lnosys -lgcc -o $@
	$(OBJCOPY) -O ihex $(PROJ_PATH).elf $(PROJ_PATH).hex
	$(OBJCOPY) -O binary $(PROJ_PATH).elf $(PROJ_PATH).bin
	$(OBJDUMP) -St $(PROJ_PATH).elf >$(PROJ_PATH).lst
	$(SIZE) $(PROJ_PATH).elf
	
	
%.o: %.c
	@echo $@
	@$(CC) $(CFLAGS) $(INC) $(DEFINES) -c -o $@ $<
	
%.o: %.S
	@echo $@
	@$(CC) $(CFLAGS) $(INC) $(DEFINES) -c -o $@ $<

  
#implement with texane on linux and stmlink CLI on windows
#program: $(PROJECT).bin
#	openocd -f $(OPENOCD_BOARD_DIR)/stm32f0discovery.cfg -f $(OPENOCD_PROC_FILE) -c "stm_flash `pwd`/$(PROJECT).bin" -c shutdown

clean:
	$(REMOVE_PROGRAM) $(CLEAN_PATH)
	$(REMOVE_FOLDER) $(CLEAN_FOLDER_PATH)
#"src\*.o" "$(HAL)\startup\*.o"
