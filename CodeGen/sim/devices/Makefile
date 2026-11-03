all: lib install

LIB = libpyblk.a
LIBDIR = $(CURDIR)/../lib
COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common
GENERIC_INC = $(COMMON_DIR)/include
TARGET_INC = ../include
TOS1A_DIR = $(PYSUPSICTRL)/CodeGen/tos1a
FIRMATA_DIR = $(PYSUPSICTRL)/CodeGen/arduinoFirmata
FIRMATA_VENDOR = $(FIRMATA_DIR)/vendor/FirmataC

.PHONY: all lib install clean

ifeq ($(OS),Windows_NT)
PLATFORM = Windows
else
PLATFORM = $(shell uname -s)
endif

INCLUDE = -I$(GENERIC_INC) -I$(TARGET_INC)

SRCALL  = $(wildcard $(COMMON_DIR)/common_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/os_compatible_dev/*.c)

# BSD sockets and termios, POSIX only
ifneq ($(PLATFORM),Windows)
SRCALL += $(wildcard $(COMMON_DIR)/os_compatible_dev/posix/*.c)

# tos1a and arduinoFirmata blocks, previously linked by sim.tmf as separate libraries
#SRCALL += $(wildcard $(TOS1A_DIR)/src/*.c)
#INCLUDE += -I$(TOS1A_DIR)/includes

# arduinoFirmata needs the FirmataC sources fetched by 'make dependencies' in $(FIRMATA_DIR)
ifneq ($(wildcard $(FIRMATA_VENDOR)/src/*.c),)
SRCALL += $(wildcard $(FIRMATA_DIR)/src/*.c)
SRCALL += $(wildcard $(FIRMATA_VENDOR)/src/*.c)
INCLUDE += -I$(FIRMATA_DIR)/includes -I$(FIRMATA_VENDOR)/includes
endif
endif

SRC = $(SRCALL)
OBJ = $(notdir $(SRC:.c=.o))

vpath %.c $(sort $(dir $(SRC)))

CC = gcc
AR = ar
DBG = -g
CC_FLAGS = -c $(DBG) $(INCLUDE)
DEFINES_SCP = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
DEFINES_LED = -D LED_SCRIPT=\"$(CWD)/led.py\"
CC_FLAGS_SCP = -c $(DBG) $(INCLUDE)  $(DEFINES_SCP)
CC_FLAGS_LED = -c $(DBG) $(INCLUDE)  $(DEFINES_LED)

led.o: led.c
	$(CC) $(CC_FLAGS_LED) $<

scope.o: scope.c
	$(CC) $(CC_FLAGS_SCP) $<

%.o: %.c
	@echo "CC: $(notdir $<)"
	@$(CC) $(CC_FLAGS) $<

lib: $(OBJ)
	@echo "AR: $(LIB)"
	@$(AR) -rcs $(LIB) $(OBJ)

install: lib
	@mkdir -p $(LIBDIR)
	@cp $(LIB) $(LIBDIR)

clean:
	@rm -f $(LIB) $(OBJ)
