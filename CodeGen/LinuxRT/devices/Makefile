all: allfiles lib install
reduced: files lib install

LIB = libpyblk.a
GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
TARGET_INC = ../include
INCLUDE = -I$(GENERIC_INC) -I$(TARGET_INC)
COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common

SRCALL = $(wildcard *.c)
SRCALL += $(wildcard $(COMMON_DIR)/CAN_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/common_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Faulhaber_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Maxon_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/posix/*.c)

OBJ = $(notdir $(SRCALL:.c=.o))

CWD = $(shell pwd)
FMUDIR = ../fmu

######DBG = -g -fPIC
DBG = -g

OBJEX = $(SRC:.c=.o)
DEFINES = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
CC_FLAGS = -c $(DBG) $(INCLUDE) 
CC_FLAGS_SCP = -c $(DBG) $(INCLUDE)  $(DEFINES)
CC_FLAGS_FMU = -c $(DBG) $(INCLUDE) -Wall -DFMI_COSIMULATION \
                             -DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
                              -I$(FMUDIR)/fmuinc/include -I$(FMUDIR)/fmuinc/parser -I$(FMUDIR)/fmuinc

allfiles:

files:
EXCLUDE = comedi_analog_input.c comedi_analog_output.c comedi_digital_input.c comedi_digital_output.c
SRC=$(filter-out $(EXCLUDE),$(SRCALL))

scope.o: scope.c
	cc $(CC_FLAGS_SCP) $<

FMUinterface.o: FMUinterface.c
	cc $(CC_FLAGS_FMU) $<

%.o: %.c
	cc $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/CAN_dev/%.c
	cc $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/common_dev/%.c
	cc $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/Faulhaber_dev/%.c
	cc $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/Maxon_dev/%.c
	cc $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/posix/%.c
	cc $(CC_FLAGS) $<

lib: $(OBJ)
	ar -r $(LIB) $(OBJ)

install:
	mv $(LIB) ../lib

clean:
	rm -f $(LIB) $(OBJ)
