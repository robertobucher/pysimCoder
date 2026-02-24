LIB = libdspic.a

all: $(LIB)

VER_TOOLS = v3.30
VER_DFP = 1.2.135

TOOL_DIR = /opt/microchip/xc-dsc/$(VER_TOOLS)/bin
DFP = $(HOME)/.mchp_packs/Microchip/dsPIC33AK-MP_DFP/$(VER_DFP)/xc16

MCU = 33AK512MPS512

MCC = adc?.c clock.c interrupt.c pins.c pwm*.c qei?.c system.c uart?.c

GCC = $(TOOL_DIR)/xc-dsc-gcc
AR = $(TOOL_DIR)/xc-dsc-ar

GCCFLAGS  = -c -mcpu=$(MCU) -mdfp=$(DFP) -O0 -Wall -omf=elf
TARGET_INC = ../include
GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
INCLUDES = -I$(GENERIC_INC) -I$(TARGET_INC)

ASMFLAGS =  -c -mcpu=$(MCU)  -mdfp=$(DFP)  -omf=elf

COMMON_DIR = $(PYSUPSICTRL)/CodeGen/Common

SRCALL = $(wildcard *.c)
SRCALL += $(wildcard *.s)
SRCALL += $(wildcard $(COMMON_DIR)/common_dev/*.c)

OBJ = $(notdir $(SRCALL:.c=.o))

%.o: %.c
	$(GCC) $< $(GCCFLAGS)  $(INCLUDES) -o $@

%.o: $(COMMON_DIR)/common_dev/%.c
	$(GCC) $< $(GCCFLAGS)  $(INCLUDES) -o $@

%.o: %.s
	$(GCC) $< $(ASMFLAGS)  -o $@

$(LIB): $(OBJ)
	$(AR) -r $(LIB) $(OBJ)  -mdfp=$(DFP) 

install: $(LIB)
	cp $(LIB) ../lib

clean:
	@echo "RM: $(LIB)"
	@echo "RM: *.o"
	@rm -f $(LIB) $(OBJ)

cleanall:
	@echo "RM: $(LIB)"
	@echo "RM: *.o"
	@rm -f $(LIB) $(OBJ) $(MCC)

