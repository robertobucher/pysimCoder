all: addfiles modules fmu full_lib arduino_firmata driver link
reduced: addfiles modules fmu lib arduino_firmata driver link

BINDIR = /usr/local/bin
CURDIR = $(shell pwd)
PYCTL = export PYSUPSICTRL=$(CURDIR)

RESBASH=$(shell grep PYSUPSICTRL $(HOME)/.bashrc)
RESALIAS=$(shell grep pysimCoder= $(HOME)/.bash_aliases)

PIPVERS=$(shell pip --version)
ifeq ($(findstring pip 23,$(PIPVERS)),pip 23)
	PIPFLAGS = --break-system-packages
endif

export PYSUPSICTRL=$(shell pwd)

addfiles: control slycot

control:
	pip install control $(PIPFLAGS)

slycot:
	pip install slycot

modules:
	cd toolbox/supsictrl; pip install .  $(PIPFLAGS); rm -rf build *.egg-info
	cd toolbox/supsisim; pip install .  $(PIPFLAGS); rm -rf build *.egg-info

fmu:
	cd CodeGen/LinuxRT/fmu; make all; make install; make clean

lib:
	cd CodeGen/LinuxRT/devices; make reduced

full_lib:
	cd CodeGen/LinuxRT/devices; make all

arduino_firmata:
	cd CodeGen/arduinoFirmata; make all

link:
	cd $(BINDIR); rm -f pyParams gen_pydev loadnrt defBlocks dataplot \
	xblk2Blk configLibs
	cp BlockEditor/gen_pydev $(BINDIR)/
	cd $(CURDIR)/BlockEditor; chmod a+x *.py
	cd $(CURDIR)/DriverNRT/; chmod a+x loadnrt
	ln -s $(CURDIR)/BlockEditor/pyParams.py $(BINDIR)/pyParams
	ln -s $(CURDIR)/BlockEditor/defBlocks.py $(BINDIR)/defBlocks
	ln -s $(CURDIR)/BlockEditor/dataplot.py $(BINDIR)/dataplot
	ln -s $(CURDIR)/BlockEditor/xblk2Blk.py $(BINDIR)/xblk2Blk
	ln -s $(CURDIR)/BlockEditor/configLibs.py $(BINDIR)/configLibs
	ln -s $(CURDIR)/DriverNRT/loadnrt $(BINDIR)/loadnrt

driver:
	cd DriverNRT; make; make install

user:
ifneq ($(findstring PYSUPSICTRL,$(RESBASH)),PYSUPSICTRL)
	echo $(PYCTL) >> ~/.bashrc
	echo 'export PYEDITOR=emacs' >> ~/.bashrc
	echo 'export PYTHONPATH=$(HOME)/Documents/PYTHON:$(CURDIR)/resources/blocks/rcpBlk' >> ~/.bashrc
	echo 'export ARDUINO_TOOLDIR=$(HOME)/.arduino15' >> ~/.bashrc
	echo 'export ARDUINO_DIR=$(HOME)/sviluppo/arduino' >> ~/.bashrc
	echo 'export SAMD21_HOME=$(CURDIR)/CodeGen/SAMD21' >> ~/.bashrc
	echo 'export STM32H7_HOME=$(CURDIR)/CodeGen/STM32H7' >> ~/.bashrc
endif

alias:
	@echo $(RESALIAS)
ifneq ($(findstring pysimCoder=, $(RESALIAS)),pysimCoder=)
	echo 'alias pysimCoder='"'$(CURDIR)/pysim-run.sh'"''  >> ~/.bash_aliases
endif

clean:
	rm -f CodeGen/LinuxRT/lib/*.a
	rm -f CodeGen/LinuxRT/fmu/lib/*.a
	cd CodeGen/LinuxRT/devices; make clean
	rm -f CodeGen/Raspberry_PI/lib/*.a
	cd CodeGen/Raspberry_PI/devices; make clean
	rm -f CodeGen/SAMD21/lib/SAMD21lib.a
	cd CodeGen/SAMD21/devices; make clean
	rm -f CodeGen/STM32H7/lib/*.a
	rm -f CodeGen/STM32H7/objects/obj/*.o
	rm -rf resources/blocks/rcpBlk/__pycache__/
	rm -rf /home/bucher/CACSD/pysimCoder/toolbox/supsictrl/build/
	rm -rf /home/bucher/CACSD/pysimCoder/toolbox/supsisim/build/
	rm -f CodeGen/arduinoFirmata/lib/*.a

############ Optional targets ############
######## Specific compiler required!!! ########
##### Install al these targets as normal user ! #####

RaspLib:
	cd CodeGen/Raspberry_PI/devices; make; make install; make clean
	cd CodeGen/Raspberry_PI/lib; \
	wget robertobucher.dti.supsi.ch/wp-content/uploads/2017/03/RaspLibs.zip; \
	unzip RaspLibs.zip; rm *.zip

SAMD21:
	cd CodeGen/SAMD21/devices;make; make install; make clean
	cd CodeGen/SAMD21/lib; wget robertobucher.dti.supsi.ch/wp-content/uploads/2017/03/core.zip; unzip core.zip; rm core.zip

STM32H7:
	cd CodeGen/STM32H7; make
	echo $(CURDIR)/CodeGen/STM32H7/objects/obj/stm32h7xx_hal_msp.o > CodeGen/STM32H7/objects/objects.list1
	echo $(CURDIR)/CodeGen/STM32H7/objects/obj/stm32h7xx_it.o >> CodeGen/STM32H7/objects/objects.list1
	echo $(CURDIR)/CodeGen/STM32H7/objects/obj/syscalls.o >> CodeGen/STM32H7/objects/objects.list1
	echo $(CURDIR)/CodeGen/STM32H7/objects/obj/sysmem.o >> CodeGen/STM32H7/objects/objects.list1
	echo $(CURDIR)/CodeGen/STM32H7/objects/obj/system_stm32h7xx_dualcore_boot_cm4_cm7.o >> CodeGen/STM32H7/objects/objects.list1



