all: dependencies lib install
reduced: dependencies lib install

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

.PHONY: all dependencies clean install

SRCALL = $(wildcard *.c)
SRCALL += $(wildcard $(COMMON_DIR)/CAN_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/common_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Faulhaber_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/Maxon_dev/*.c)
SRCALL += $(wildcard $(COMMON_DIR)/posix/*.c)

dependencies:
	@mkdir -p $(GENERATED_INC)

CWD = $(shell pwd)

######DBG = -g -fPIC
DBG = -g

CC = arm-linux-gnueabihf-gcc
AR = arm-linux-gnueabihf-ar
OBJEX = $(SRC:.c=.o)
INCLUDE =  -I$(GENERIC_INC) -I$(TARGET_INC)
CC_FLAGS = -c $(DBG) $(INCLUDE)
CC_FLAGS += -I$(GENERATED_INC)
ifeq ($(SHV),1)
CC_FLAGS += -I$(COMMON_DIR)/shv/include
endif
CC_FLAGS += -D CG_WITH_ENV_HOST_ADDR

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

SRC=$(filter-out $(EXCLUDE),$(SRCALL))
OBJ = $(notdir $(SRCALL:.c=.o))

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
