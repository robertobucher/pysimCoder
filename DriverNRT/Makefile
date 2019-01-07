obj-m:= nrt.o

KDIR = /lib/modules/$(shell uname -r)/build
MDIR = /lib/modules/$(shell uname -r)/kernel/drivers/misc

PWD  = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.mod.* *.o *.ko

install:
	cp nrt.ko $(MDIR)

dev:
	mknod -m0666 /dev/nrt c 20 0
  
