all: lib 
reduced: files lib install

LIB = libPipyblk.a
RTPYINC = ../include
SRCALL = $(wildcard *.c)
OBJ = $(SRCALL:.c=.o)
CWD = $(shell pwd)

######DBG = -g -fPIC
DBG = -g

CC = arm-linux-gnueabihf-gcc
AR = arm-linux-gnueabihf-ar
OBJEX = $(SRC:.c=.o)
CC_FLAGS = -c $(DBG) -I$(RTPYINC) 

%.o: %.c
	$(CC) $(CC_FLAGS) $<

lib: $(OBJ)
	$(AR) -r $(LIB) $(OBJ)

install:
	mv $(LIB) ../lib

clean:
	rm -f $(LIB) $(OBJ)
