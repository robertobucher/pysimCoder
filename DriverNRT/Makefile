obj-m:= nrt.o

KDIR = /lib/modules/$(shell uname -r)/build
MDIR = /lib/modules/$(shell uname -r)/kernel/drivers/misc

PWD  = $(shell pwd)

# Check kernel version for using subdirs
VERSION= $(shell uname -r)
KVERSION  := $(shell echo $(VERSION) | sed -e 's/^\([1-9][0-9]*\)\..*/\1/' )
KMAJOR := $(shell echo $(VERSION) | sed -e 's/^$(KVERSION)\.0*\([0-9][0-9]*\).*/\1/' )
KVERNUM := $(shell expr $(KVERSION) \* 10000 + $(KMAJOR) \* 100)

define USESUBDIRS
$(shell [ $(KVERNUM) -ge 20605 ] && [ $(KVERNUM) -lt 50000 ] && echo yes)
endef
ifneq ($(USESUBDIRS),yes)
SUBD = M=$(PWD)
else
SUBD = SUBDIRS=$(PWD)
endif
# USESUBDIRS

default:
	@echo $(KVERNUM)
	$(MAKE) -C $(KDIR) $(SUBD) modules

clean:
	rm -f *.mod.* *.o *.ko

install:
	cp nrt.ko $(MDIR)

dev:
	mknod -m0666 /dev/nrt c 20 0
  
