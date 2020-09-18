LIB = libHAL.a

all: $(LIB)

SRCALL1 = $(wildcard *.c)
OBJ1 = $(SRCALL1:.c=.o)
SRCALL2 = $(wildcard *.s)
OBJ2 = $(SRCALL2:.s=.o)
OBJ = $(OBJ1) $(OBJ2)

BIN_DIR = /usr/bin

GCC = $(BIN_DIR)/arm-none-eabi-gcc
AR = $(BIN_DIR)/arm-none-eabi-ar
CPU = -mcpu=cortex-m7

GCC_FLAGS1 = \
-std=gnu11 -g3

GCC_FLAGS2 = \
-O0 -ffunction-sections -fdata-sections -Wall -fstack-usage \
-MMD -MP \
--specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb

S_FLAGS = \
-x assembler-with-cpp \
--specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb

DEFINES = \
-DUSE_HAL_DRIVER \
-DCORE_CM7 \
-DDEBUG \
-DSTM32H745xx

INCLUDES = \
-I$(STM32H7_HOME)/objects/inc

%.o: %.c
	$(GCC) $< $(CPU) $(GCC_FLAGS1) $(DEFINES) -c $(INCLUDES) $(GCC_FLAGS2) -o $@

%.o: %.s
	$(GCC) -c $< $(CPU) -g3 $(S_FLAG) -o $@

$(LIB): $(OBJ)
	$(AR) -r $(LIB) $(OBJ)

install: $(LIB)
	mv $(LIB) $(STM32H7_HOME)/lib

update:
	cp $(STM32H7_HOME)/refProj/Inc/stm32h7xx_hal_conf.h ../inc
	cp  $(STM32H7_HOME)/refProj/Inc//stm32h7xx_it.h ../inc
	cp $(STM32H7_HOME)/refProj/Startup/startup_stm32h745zitx.s .
	cp  $(STM32H7_HOME)/refProj/Inc//main.h ../inc

clean:
	rm -f *.o  *.d *.map *.elf *.bin *.hex  *.su ../obj/*.d ../obj/*.su

