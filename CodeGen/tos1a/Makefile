all: clean install

LIB = tos1apyblk.a

GENERIC_INC = $(PYSUPSICTRL)/CodeGen/Common/include
TARGET_SRC = src
TARGET_INC = #vendor/FirmataC/includes
LIB_INC = includes
INCLUDE = -I$(GENERIC_INC) -I$(TARGET_INC) -I$(LIB_INC)

SRC_ALL = $(wildcard $(TARGET_SRC)/*.c)
#SRC_ALL += $(wildcard vendor/FirmataC/src/*.c)

OBJ =  $(SRC_ALL:.c=.o)

CWD = $(shell pwd)

DBG = -g
CC_FLAGS = -c $(DBG) $(INCLUDE)

allfiles:

files:
EXCLUDE = # ./src/[xxx].c ....
SRC=$(filter-out $(EXCLUDE),$(SRC_ALL))


%.o: %.c
	cc $(CC_FLAGS) $<

install: $(OBJ)
	mkdir "lib"
	ar -r $(LIB) $(notdir $(OBJ))
	rm -f *.o
	mv $(LIB) lib

clean:
	rm -rf lib

clean_novendor:
	rm -rf lib
