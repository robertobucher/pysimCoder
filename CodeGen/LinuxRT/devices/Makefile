all: dependencies allfiles lib install
reduced: files lib install

LIB = libpyblk.a
GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
TARGET_INC = ../include
INCLUDE = -I$(GENERIC_INC) -I$(TARGET_INC)
COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common
EXT_LIBS = $(PYSUPSICTRL)/ExtLibs

.PHONY: all dependencies clean install allfiles

SRCALL = $(wildcard *.c)
SRCALL += $(wildcard $(COMMON_DIR)/CAN_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/common_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Faulhaber_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Maxon_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/posix/*.c)

ifeq ($(SHV),1)
EXT_SHV_LIB = $(EXT_LIBS)/libshv/libshvchainpack/c

SRCALL += $(EXT_SHV_LIB)/cchainpack.c $(EXT_SHV_LIB)/ccpcp.c \
					$(EXT_SHV_LIB)/ccpcp_convert.c $(EXT_SHV_LIB)/ccpon.c \
					$(EXT_SHV_LIB)/proof-of-concept-code/pack_double.c
SRCALL += $(EXT_LIBS)/ulut/ulut/ul_gavlprim.c
SRCALL += $(EXT_LIBS)/ulut/ulut/ul_gsacust.c
SRCALL += $(wildcard $(COMMON_DIR)/shv/*.c)
endif

OBJ = $(notdir $(SRCALL:%.c=%.o))

CWD = $(shell pwd)
FMUDIR = ../fmu

######DBG = -g -fPIC
DBG = -g

CC ?= cc
AR ?= ar
OBJEX = $(SRC:.c=.o)
DEFINES = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
CC_FLAGS = -c $(DBG) $(INCLUDE)
ifeq ($(SHV),1)
CC_FLAGS += -I$(COMMON_DIR)/shv/include
CC_FLAGS += -I$(EXT_LIBS)/libshv/libshvchainpack/c
CC_FLAGS += -I$(EXT_LIBS)/ulut
endif
CC_FLAGS += -D CG_WITH_ENV_HOST_ADDR
CC_FLAGS_SCP = -c $(DBG) $(INCLUDE)  $(DEFINES)
CC_FLAGS_FMU = -c $(DBG) $(INCLUDE) -Wall -DFMI_COSIMULATION \
                             -DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
                              -I$(FMUDIR)/fmuinc/include -I$(FMUDIR)/fmuinc/parser -I$(FMUDIR)/fmuinc

dependencies:
ifeq ($(SHV),1)
	cd $(EXT_LIBS) && if [ ! -e libshv ]; then git clone https://github.com/silicon-heaven/libshv; fi
	cd $(EXT_LIBS) && if [ ! -e ulut ]; then git clone https://git.code.sf.net/p/ulan/ulut; fi
endif
	cd $(PYSUPSICTRL)/CodeGen/LinuxRT/devices

allfiles:

files:
EXCLUDE = comedi_analog_input.c comedi_analog_output.c comedi_digital_input.c comedi_digital_output.c
SRC=$(filter-out $(EXCLUDE),$(SRCALL))

scope.o: scope.c
	$(CC) $(CC_FLAGS_SCP) $<

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

ifeq ($(SHV),1)
cchainpack.o: $(EXT_SHV_LIB)/cchainpack.c
	$(CC) $(CC_FLAGS) $<

ccpcp.o: $(EXT_SHV_LIB)/ccpcp.c
	$(CC) $(CC_FLAGS) $<

ccpcp_convert.o: $(EXT_SHV_LIB)/ccpcp_convert.c
	$(CC) $(CC_FLAGS) $<

ccpon.o: $(EXT_SHV_LIB)/ccpon.c
	$(CC) $(CC_FLAGS) $<

pack_double.o: $(EXT_SHV_LIB)/proof-of-concept-code/pack_double.c
	$(CC) $(CC_FLAGS) $<

ul_gavlprim.o: $(EXT_LIBS)/ulut/ulut/ul_gavlprim.c
	$(CC) $(CC_FLAGS) $<

ul_gsacust.o: $(EXT_LIBS)/ulut/ulut/ul_gsacust.c
	$(CC) $(CC_FLAGS) $<

%.o: $(COMMON_DIR)/shv/%.c
	$(CC) $(CC_FLAGS) $<
endif

lib: $(OBJ)
	$(AR) -r $(LIB) $(OBJ)

install:
	mv $(LIB) ../lib

clean:
	rm -rf $(EXT_LIBS)/libshv
	rm -rf $(EXT_LIBS)/ulut
	rm -f $(LIB) $(OBJ)
