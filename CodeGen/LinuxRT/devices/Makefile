all: dependencies allfiles lib install
reduced: files lib install

LIB = libpyblk.a
LIBDIR=$(CURDIR)/../lib
GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
TARGET_INC = ../include
INCLUDE = -I$(GENERIC_INC) -I$(TARGET_INC)
COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common
EXT_LIBS = $(PYSUPSICTRL)/ExtLibs
GENERATED_INC = $(CURDIR)/../include-generated

OMK_COMPILED_INC_PATH = $(GENERATED_INC)
OMK_COMPILED_LIB_PATH = $(LIBDIR)

.PHONY: all dependencies clean install allfiles

SRCALL = $(wildcard *.c)
SRCALL += $(wildcard $(COMMON_DIR)/CAN_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/common_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Faulhaber_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Maxon_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/posix/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/AR2INO/*.c)

dependencies:
	@mkdir -p $(GENERATED_INC)

OBJ = $(notdir $(SRCALL:%.c=%.o))

CWD = $(shell pwd)
FMUDIR = ../fmu

######DBG = -g -fPIC
DBG = -g

CC ?= cc
AR ?= ar
OBJEX = $(SRC:.c=.o)
DEFINES_SCP = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
DEFINES_LED = -D LED_SCRIPT=\"$(CWD)/led.py\"
CC_FLAGS = -c $(DBG) $(INCLUDE)
CC_FLAGS += -I$(GENERATED_INC)
ifeq ($(SHV),1)
CC_FLAGS += -I$(COMMON_DIR)/shv/include
endif
CC_FLAGS += -D CG_WITH_ENV_HOST_ADDR
CC_FLAGS_SCP = -c $(DBG) $(INCLUDE)  $(DEFINES_SCP)
CC_FLAGS_LED = -c $(DBG) $(INCLUDE)  $(DEFINES_LED)
CC_FLAGS_FMU = -c $(DBG) $(INCLUDE) -Wall -DFMI_COSIMULATION \
                             -DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
                              -I$(FMUDIR)/fmuinc/include -I$(FMUDIR)/fmuinc/parser -I$(FMUDIR)/fmuinc

EXTLIB_CFLAGS=$(CC_FLAGS)
EXTLIB_CXXFLAGS=$(CC_FLAGS)
EXTLIB_LDFLAGS=$(CC_FLAGS)

ifeq ($(SHV),1)
SRCALL += $(wildcard $(COMMON_DIR)/shv/*.c)

CONFIG_SHV_LIBS4C_PLATFORM="linux"
include $(COMMON_DIR)/rules/make-ulut.inc
include $(COMMON_DIR)/rules/make-shv-libs4c.inc
dependencies: make-ulut make-shv-libs4c
endif

allfiles:

files:
EXCLUDE = comedi_analog_input.c comedi_analog_output.c comedi_digital_input.c comedi_digital_output.c
SRC=$(filter-out $(EXCLUDE),$(SRCALL))

scope.o: scope.c
	$(CC) $(CC_FLAGS_SCP) $<

led.o: led.c
	$(CC) $(CC_FLAGS_LED) $<

FMUinterface.o: FMUinterface.c
	$(CC) $(CC_FLAGS_FMU) $<

%.o: %.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/CAN_dev/%.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/common_dev/%.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/Faulhaber_dev/%.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/Maxon_dev/%.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/posix/%.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/AR2INO/%.c
	$(CC) $(CC_FLAGS) $<

ifeq ($(SHV),1)
%.o: $(COMMON_DIR)/shv/%.c
	$(CC) $(CC_FLAGS) $<
endif

lib: $(OBJ)
	$(AR) -r $(LIB) $(OBJ)

install:
	mkdir -p ../lib
	cp $(LIB) ../lib

clean:
	@echo "RM: _build _compiled *.omk-default include-generated"
	@echo "RM: $(LIB)"
	@echo "RM: *.o"
	@rm -rf _build _compiled *.omk-default include-generated
	@rm -f $(LIB) $(OBJ)
