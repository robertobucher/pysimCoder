all: allfiles lib install
reduced: files lib install

LIB = libpyblk.a
RTPYINC = ../include
SRCALL = $(wildcard *.c)
OBJ = $(SRCALL:.c=.o)
CWD = $(shell pwd)
FMUDIR = ../fmu

######DBG = -g -fPIC
DBG = -g

OBJEX = $(SRC:.c=.o)
DEFINES = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
CC_FLAGS = -c $(DBG) -I$(RTPYINC) 
CC_FLAGS_SCP = -c $(DBG) -I$(RTPYINC)  $(DEFINES)
CC_FLAGS_FMU = -c $(DBG) -I$(RTPYINC) -Wall -DFMI_COSIMULATION \
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

lib: $(OBJ)
	ar -r $(LIB) $(OBJ)

install:
	mv $(LIB) ../lib

clean:
	rm -f $(LIB) $(OBJ)
