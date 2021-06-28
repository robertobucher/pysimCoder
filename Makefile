all: script addfiles modules fmu full_lib driver link
reduced: script addfiles modules fmu lib driver link

BINDIR = /usr/local/bin
CURDIR = $(shell pwd)
CWD = $(shell pwd)
PYCTL = export PYSUPSICTRL=$(CWD)
PYSUPSICTRL=$(shell pwd)

addfiles: control slycot

control:
	git clone https://github.com/python-control/python-control.git
	cd python-control; python setup.py install
	rm -rf python-control 

slycot:
	git clone --recurse-submodules https://github.com/python-control/Slycot
	cd Slycot; python setup.py install
	rm -rf Slycot

modules:
	cd toolbox/supsictrl; python setup.py install; python setup.py clean --all
	cd toolbox/supsisim; python setup.py install; python setup.py clean --all

fmu:
	cd CodeGen/LinuxRT/fmu; make all; make install

lib:
	cd CodeGen/LinuxRT/devices; make reduced

full_lib:
	cd CodeGen/LinuxRT/devices; make all

link:
	cd $(BINDIR); rm -f pysimCoder pysimCoder.py pyParams gen_pydev loadnrt defBlocks dataplot xblk2Blk
	cp BlockEditor/gen_pydev $(BINDIR)/
	cd $(CURDIR)/BlockEditor; chmod a+x *.py
	cd $(CURDIR)/DriverNRT/; chmod a+x loadnrt
	ln -s $(CURDIR)/pysimCoder $(BINDIR)/pysimCoder
	ln -s $(CURDIR)/BlockEditor/pysimCoder.py $(BINDIR)/pysimCoder.py
	ln -s $(CURDIR)/BlockEditor/pyParams.py $(BINDIR)/pyParams
	ln -s $(CURDIR)/BlockEditor/defBlocks.py $(BINDIR)/defBlocks
	ln -s $(CURDIR)/BlockEditor/dataplot.py $(BINDIR)/dataplot
	ln -s $(CURDIR)/BlockEditor/xblk2Blk.py $(BINDIR)/xblk2Blk
	ln -s $(CURDIR)/DriverNRT/loadnrt $(BINDIR)/loadnrt

driver:
	cd DriverNRT; make; make install

user:
	echo $(PYCTL) >> ~/.bashrc
	echo 'export PYEDITOR=emacs' >> ~/.bashrc
	echo 'export PYTHONPATH=$(HOME)/Documents/PYTHON:$(CURDIR)/resources/blocks/rcpBlk' >> ~/.bashrc
	echo 'export ARDUINO_TOOLDIR=$(HOME)/.arduino15' >> ~/.bashrc
	echo 'export ARDUINO_DIR=$(HOME)/sviluppo/arduino' >> ~/.bashrc
	echo 'export SAMD21_HOME=$(CURDIR)/CodeGen/SAMD21' >> ~/.bashrc
	echo 'export STM32H7_HOME=$(CURDIR)/CodeGen/STM32H7' >> ~/.bashrc

script:
	echo $(PYCTL) > pysimCoder
	echo 'export PYEDITOR=emacs' >> pysimCoder
	echo 'export PYTHONPATH=$(CURDIR)/resources/blocks/rcpBlk' >> pysimCoder
	cat launch.txt >> pysimCoder
	chmod a+x pysimCoder

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

############ Optional targets ############
######## Specific compiler required!!! ########
##### Install al these targets as normal user ! #####

RaspLib:
	cd CodeGen/Raspberry_PI/devices; make; make install; make clean
	cd CodeGen/Raspberry_PI/lib; wget robertobucher.dti.supsi.ch/wp-content/uploads/2017/03/RaspLibs.zip; unzip RaspLibs.zip; rm *.zip

SAMD21:
	cd CodeGen/SAMD21/devices;make; make install; make clean
	cd CodeGen/SAMD21/lib; wget robertobucher.dti.supsi.ch/wp-content/uploads/2017/03/core.zip; unzip core.zip; rm core.zip

STM32H7:
	cd CodeGen/STM32H7; make
	echo $(CWD)/CodeGen/STM32H7/objects/obj/stm32h7xx_hal_msp.o > CodeGen/STM32H7/objects/objects.list1
	echo $(CWD)/CodeGen/STM32H7/objects/obj/stm32h7xx_it.o >> CodeGen/STM32H7/objects/objects.list1
	echo $(CWD)/CodeGen/STM32H7/objects/obj/syscalls.o >> CodeGen/STM32H7/objects/objects.list1
	echo $(CWD)/CodeGen/STM32H7/objects/obj/sysmem.o >> CodeGen/STM32H7/objects/objects.list1
	echo $(CWD)/CodeGen/STM32H7/objects/obj/system_stm32h7xx_dualcore_boot_cm4_cm7.o >> CodeGen/STM32H7/objects/objects.list1



