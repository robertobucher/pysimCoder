all: allfiles lib install
reduced: files lib install

LIB = libpyblk.a
RTPYINC = ../include
SRCALL = $(wildcard *.c)
OBJ = $(SRCALL:.c=.o)
CWD = $(shell pwd)

######DBG = -g -fPIC
DBG = -g


OBJEX = $(SRC:.c=.o)
DEFINES = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
CC_FLAGS = -c $(DBG) -I$(RTPYINC)  $(DEFINES)

allfiles:
INCFMI = 

files:
EXCLUDE = comedi_analog_input.c comedi_analog_output.c comedi_digital_input.c comedi_digital_output.c
SRC=$(filter-out $(EXCLUDE),$(SRCALL))

%.o: %.c
	cc $(CC_FLAGS) $<

lib: $(OBJ)
	ar -r $(LIB) $(OBJ)

install:
	mv $(LIB) ../lib

clean:
	rm -f $(LIB) $(OBJ)
