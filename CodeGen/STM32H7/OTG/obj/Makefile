LIB = libOTG.a

all: install

BIN_DIR = /usr/bin

AR = $(BIN_DIR)/arm-none-eabi-ar

$(LIB):
	$(AR) -r $(LIB) *.o

install: $(LIB)
	mv $(LIB) $(STM32H7_HOME)/lib

clean:
	rm -f *.o

