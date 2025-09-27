all: dependencies allfiles lib install
reduced: files lib install

LIB = libpyblk.a
LIBDIR=$(CURDIR)/../lib
GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
TARGET_INC = ../include
INCLUDE =  -I$(GENERIC_INC) -I$(TARGET_INC)

COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common
EXT_LIBS = $(PYSUPSICTRL)/ExtLibs
GENERATED_INC = $(CURDIR)/../include-generated

OMK_COMPILED_INC_PATH = $(GENERATED_INC)
OMK_COMPILED_LIB_PATH = $(LIBDIR)

NUTTX_EXPORT = $(CURDIR)/../nuttx-export

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

dependencies:
	@mkdir -p $(GENERATED_INC)

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
ifeq ($(CONFIG_SPI_EXCHANGE),y)
SRCALL += heater.c
SRCALL += RLC.c
SRCALL += DCMOT.c
SRCALL += nuttx_p3m_spi.c
SRCALL += pm_mc1_spi.c
endif
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
CC_FLAGS += -I$(GENERATED_INC)
ifeq ($(SHV),1)
CC_FLAGS += -I$(COMMON_DIR)/shv/include
endif
CC_FLAGS_SCP = -c $(DBG) $(INCLUDE) $(CFLAGS) $(DEFINES)
CC_FLAGS_FMU = -c $(DBG) $(INCLUDE) $(CFLAGS) -Wall -DFMI_COSIMULATION \
                             -DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
                              -I$(FMUDIR)/fmuinc/include -I$(FMUDIR)/fmuinc/parser -I$(FMUDIR)/fmuinc

EXTLIB_CFLAGS=$(CC_FLAGS)
EXTLIB_CXXFLAGS=$(CC_FLAGS)
EXTLIB_LDFLAGS=$(CC_FLAGS)

ifeq ($(SHV),1)
# The Python generated SHV Code uses the getenv to retrieve
# SHV parameters that override the default ones. Unfortunately for us,
# getenv and setenv might be disabled. Raise an error if this CONFIG is on.
ifneq ($(shell grep CONFIG_DISABLE_ENVIRON "$(NUTTX_EXPORT)/include/nuttx/config.h"),)
$(error "In order for SHV to work, NuttX build must have CONFIG_DISABLE_ENVIRON unset!")
endif

SRCALL += $(wildcard $(COMMON_DIR)/shv/*.c)

# If the SHV tree liraries are not provided by NuttX build local copy
ifeq ($(wildcard $(NUTTX_EXPORT)/include/shv/tree/shv_tree.h),)
CONFIG_SHV_LIBS4C_PLATFORM="nuttx"
include $(COMMON_DIR)/rules/make-ulut.inc
include $(COMMON_DIR)/rules/make-shv-libs4c.inc
dependencies: make-ulut make-shv-libs4c
endif
endif

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
	cp $(LIB) ../lib

clean:
	@echo "RM: _build _compiled *.omk-default include-generated"
	@echo "RM: $(LIB)"
	@echo "RM: *.o"
	@rm -rf _build _compiled *.omk-default include-generated
	@rm -f $(LIB) $(OBJ)
