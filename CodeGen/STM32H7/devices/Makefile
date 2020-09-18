LIB = libpyBlk.a

all: $(LIB)

SRCALL = $(wildcard *.c)
OBJ = $(SRCALL:.c=.o)

BIN_DIR = /usr/bin

GCC =$(BIN_DIR)/arm-none-eabi-gcc
AR = $(BIN_DIR)/arm-none-eabi-ar
CPU = -mcpu=cortex-m7

GCC_FLAGS1 = \
-std=gnu11 -g3

GCC_FLAGS2 = \
-O0 -ffunction-sections -fdata-sections -Wall -fstack-usage \
-MMD -MP \
--specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb

DEFINES = \
-DUSE_HAL_DRIVER \
-DCORE_CM7 \
-DDEBUG \
-DSTM32H745xx

INCLUDES = \
-I$(STM32H7_HOME)/objects/inc \
-I$(STM32H7_HOME)/include \
-I$(STM32H7_HOME)/OTG/STM32_USB_Device_Library/Core/Inc \
-I$(STM32H7_HOME)/OTG/STM32_USB_Device_Library/Class/CDC/Inc \
-I$(STM32H7_HOME)/OTG/USB_DEVICE/Target \
-I$(STM32H7_HOME)/OTG/USB_DEVICE/App 

%.o: %.c
	$(GCC) $< $(CPU) $(GCC_FLAGS1) $(DEFINES) -c $(INCLUDES) $(GCC_FLAGS2) -o $@

$(LIB): $(OBJ)
	$(AR) -r $(LIB) $(OBJ)

install: $(LIB)
	mv $(LIB) $(STM32H7_HOME)/lib

clean:
	rm -f *.o *.d  *.su 

