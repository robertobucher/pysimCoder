all: dependencies allfiles lib install
reduced: files lib install

LIB = libpyblk.a
GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
TARGET_INC = ../include
INCLUDE =  -I$(GENERIC_INC) -I$(TARGET_INC)

COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common
EXT_LIBS = $(PYSUPSICTRL)/ExtLibs

NUTTX_EXPORT = $(PWD)/../nuttx-export

ifndef NUTTX_EXPORT
$(warning Specify NUTTX_EXPORT)
$(warning make_rtw NUTTX_EXPORT=/path/to/nuttx-export)
$(error NUTTX_EXPORT not defined, cannot continue)
endif

include $(NUTTX_EXPORT)/scripts/Make.defs
include $(NUTTX_EXPORT)/.config

SRCALL = $(wildcard $(COMMON_DIR)/common_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/posix/*.c)
SRCALL += serialOut.c

ifeq ($(SHV),1)
EXT_SHV_LIB = $(EXT_LIBS)/libshv/libshvchainpack/c

SRCALL += $(EXT_SHV_LIB)/cchainpack.c $(EXT_SHV_LIB)/ccpcp.c \
					$(EXT_SHV_LIB)/ccpcp_convert.c $(EXT_SHV_LIB)/ccpon.c \
					$(EXT_SHV_LIB)/proof-of-concept-code/pack_double.c
SRCALL += $(EXT_LIBS)/ulut/ulut/ul_gavlprim.c
SRCALL += $(EXT_LIBS)/ulut/ulut/ul_gsacust.c
SRCALL += $(wildcard $(COMMON_DIR)/shv/*.c)
endif

ifeq ($(CONFIG_ADC),y)
SRCALL += nuttx_ADC.c
endif

ifeq ($(CONFIG_DAC),y)
SRCALL += nuttx_DAC.c
endif

ifeq ($(CONFIG_PWM),y)
SRCALL += nuttx_PWM.c
endif

ifeq ($(CONFIG_DEV_GPIO),y)
SRCALL += nuttxDI.c \
nuttxDO.c
endif

ifeq ($(CONFIG_SENSORS_QENCODER),y)
SRCALL += nuttxENC.c
endif

ifeq ($(CONFIG_SPI),y)
SRCALL += heater.c
SRCALL += RLC.c
SRCALL += DCMOT.c
SRCALL += nuttx_p3m_spi.c
SRCALL += pm_mc1_spi.c
endif

ifeq ($(CONFIG_SENSORS_DHTXX),y)
SRCALL += sensors/nuttxDHTXX.c
endif

ifeq ($(CONFIG_MICROROSLIB),y)
SRCALL += microros.c
endif

have_can =
ifeq ($(CONFIG_CAN),y)
	have_can = yes
endif
ifeq ($(CONFIG_NET_CAN),y)
	have_can = yes
endif

ifdef have_can
SRCALL += canopen.c
SRCALL += $(wildcard $(COMMON_DIR)/CAN_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Faulhaber_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Maxon_dev/*.c)
endif

OBJ = $(notdir $(SRCALL:.c=.o))

CWD = $(shell pwd)

NUTTX_INCLUDES = -isystem $(NUTTX_EXPORT)/include

SYSTEM_LIBS += --start-group $(LDLIBS) $(EXTRA_LIBS) --end-group
ELF_MODULE_LIBS = --start-group $(EXTRA_LIBS) --end-group

LDFLAGS += -L $(NUTTX_EXPORT)/libs

ifneq ($(shell $(CC) --target-help | grep -e '-mlong-calls\b'),)
TARGET_ARCH_LONG_CALLS = -mlong-calls
endif

TARGET_ARCH_FLAGS ?= $(ARCHCFLAGS) $(ARCHCPUFLAGS) \
	$(TARGET_ARCH_LONG_CALLS) -fno-common -DWITHOUT_MLOCK

DEFAULT_OPT_OPTS ?= $(ARCHOPTIMIZATION)

ifndef OPT_OPTS
OPT_OPTS = $(DEFAULT_OPT_OPTS)
endif

LD_SCRIPT = $(NUTTX_EXPORT)/scripts/$(LDNAME)

LDFLAGS += -T $(LD_SCRIPT)

LDFLAGS  += --entry=__start -nostartfiles -nodefaultlibs

ELF_FILE_LDSCRIPT ?= $(wildcard $(NUTTX_EXPORT)/scripts/gnu-elf.ld)

INCLUDES += $(NUTTX_INCLUDES)

CFLAGS += $(TARGET_ARCH_FLAGS) $(ARCHWARNINGS) $(OPT_OPTS) $(INCLUDES)

CXXFLAGS = $(TARGET_ARCH_FLAGS) $(ARCHCXXFLAGS) $(ARCHWARNINGSXX) $(OPT_OPTS) $(INCLUDES)

OBJEX = $(SRC:.c=.o)
DEFINES = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
CC_FLAGS = -c $(DBG) $(INCLUDE) $(CFLAGS)
ifeq ($(SHV),1)
CC_FLAGS += -I$(COMMON_DIR)/shv/include
CC_FLAGS += -I$(EXT_LIBS)/libshv/libshvchainpack/c
CC_FLAGS += -I$(EXT_LIBS)/ulut
endif
CC_FLAGS_SCP = -c $(DBG) $(INCLUDE) $(CFLAGS) $(DEFINES)
CC_FLAGS_FMU = -c $(DBG) $(INCLUDE) $(CFLAGS) -Wall -DFMI_COSIMULATION \
                             -DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
                              -I$(FMUDIR)/fmuinc/include -I$(FMUDIR)/fmuinc/parser -I$(FMUDIR)/fmuinc

dependencies:
ifeq ($(SHV),1)
	cd $(EXT_LIBS) && if [ ! -e libshv ]; then git clone https://github.com/silicon-heaven/libshv; fi
	cd $(EXT_LIBS) && if [ ! -e ulut ]; then git clone https://git.code.sf.net/p/ulan/ulut; fi
endif
	cd $(PYSUPSICTRL)/CodeGen/nuttx/devices

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

%.o: sensors/%.c
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

%.o: $(COMMON_DIR)/shv/%.c
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

lib: $(OBJ)
	$(AR) $(LIB) $(OBJ)

install:
	mkdir -p ../lib
	mv $(LIB) ../lib

clean:
	rm -rf $(EXT_LIBS)/libshv
	rm -rf $(EXT_LIBS)/ulut
	rm -f $(LIB) $(OBJ)
