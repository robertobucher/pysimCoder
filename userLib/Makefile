all: lib

LIB = userpyblk.a
RTPYINC = ./
SRCALL = $(wildcard *.c)
OBJ = $(SRCALL:.c=.o)

######DBG = -g -fPIC
DBG = -g

OBJEX = $(SRC:.c=.o)
#CC_FLAGS = -c $(DBG) -I$(RTPYINC)

CC_FLAGS = -c $(DBG) -I$(RTPYINC)  $(INCFMI)


%.o: %.c
	cc $(CC_FLAGS) $<

lib: $(OBJ)
	ar -r $(LIB) $(OBJ)

clean:
	rm -f $(LIB) $(OBJ)
