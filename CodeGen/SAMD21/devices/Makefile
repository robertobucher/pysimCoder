LIB =  SAMD21lib.a

all: $(LIB)

PYCODEGEN = $(PYSUPSICTRL)/CodeGen
INODIR = $(ARDUINO_TOOLDIR)/packages/arduino/tools

SRCALL1 = $(wildcard *.cpp)
OBJ1 = $(SRCALL1:.cpp=.o)
SRCALL2 = $(wildcard *.c)
OBJ2 = $(SRCALL2:.c=.o)

OBJ = $(OBJ1) $(OBJ2)

# Directories
# Tools
BIN_DIR =/usr/bin
MBCDIR = $(ARDUINO_TOOLDIR)/packages/briki/hardware/mbc-wb/2.1.1
INODIR = $(ARDUINO_TOOLDIR)/packages/arduino/tools

GCC = $(BIN_DIR)/arm-none-eabi-gcc
G++ =$(BIN_DIR)/arm-none-eabi-g++
CPU = -mcpu=cortex-m0plus

LD = $(BIN_DIR)/arm-none-eabi-gcc
OBJCP =  $(BIN_DIR)/arm-none-eabi-objcopy
SIZE = $(BIN_DIR)/arm-none-eabi-size

MBCTOOL = $(PACKAGES)/briki/tools/mbctool/2.0.0/bin/mbctool

# Command flags
GCC_FLAGS = \
-mthumb -c -g -Os -w -std=gnu++11 -ffunction-sections -fdata-sections \
-fno-threadsafe-statics -nostdlib \
--param max-inline-insns-single=500 -fno-rtti -fno-exceptions -MMD 

DEFINES = \
-DBRIKI_MBC_WB_SAMD -DATCA_PRINTF -DATCA_USE_RTOS_TIMER \
-DATCA_HAL_I2C -DF_CPU=48000000L -DARDUINO=10810 -DBRIKI_ABC \
-DARDUINO_ARCH_MBC-WB -DARDUINO_ARCH_SAMD -DBRIKI_ABC -D__SAMD21G18A__ \
-DUSB_VID=0x3112 -DUSB_PID=0x0001 -DUSBCON \
"-DUSB_MANUFACTURER=\"Meteca SA\"" "-DUSB_PRODUCT=\"Briki MBC-WB\"" 

INCLUDES = \
-I$(MBCDIR)/cores/samd21 \
-I$(ARDUINO_DIR)/hardware/arduino/acr/cores/arduino \
-I$(INODIR)/CMSIS/4.5.0/CMSIS/Include/ \
-I$(INODIR)/CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL/ \
-I$(SAMD21_HOME)/include

%.o: %.c
	$(GCC) $< $(CPU) $(GCC_FLAGS) $(DEFINES) $(INCLUDES) -o $@

%.o: %.cpp
	$(G++) $< $(CPU) $(GCC_FLAGS) $(DEFINES) $(INCLUDES) -o $@

$(LIB): $(OBJ)
	$(BIN_DIR)/arm-none-eabi-ar -r $(LIB) $(OBJ)

install:  $(LIB)
	mv $(LIB) ../lib

clean:
	rm -f *.o *.d *.map *.elf *.bin *.hex *~


