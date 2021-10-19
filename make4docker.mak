all: addfiles modules fmu lib link user alias

BINDIR = /usr/local/bin
CURDIR = $(shell pwd)
PYCTL = export PYSUPSICTRL=$(CURDIR)

export PYSUPSICTRL=$(shell pwd)

addfiles: control slycot

control:
	git clone https://github.com/python-control/python-control.git
	cd python-control; python3 setup.py install
	rm -rf python-control 

slycot:
	pip install scikit-build
	git clone --recurse-submodules https://github.com/python-control/Slycot
	cd Slycot; python3 setup.py install
	rm -rf Slycot

modules:
	cd toolbox/supsictrl; python3 setup.py install; python3 setup.py clean --all
	cd toolbox/supsisim; python3 setup.py install; python3 setup.py clean --all

fmu:
	cd CodeGen/LinuxRT/fmu; make all; make install; make clean

lib: 
	cd CodeGen/LinuxRT/devices; make;make clean

link:
	cd $(BINDIR); rm -f pyParams gen_pydev loadnrt defBlocks dataplot xblk2Blk
	cp BlockEditor/gen_pydev $(BINDIR)/
	cd $(CURDIR)/BlockEditor; chmod a+x *.py
	cd $(CURDIR)/DriverNRT/; chmod a+x loadnrt
	ln -s $(CURDIR)/BlockEditor/pyParams.py $(BINDIR)/pyParams
	ln -s $(CURDIR)/BlockEditor/defBlocks.py $(BINDIR)/defBlocks
	ln -s $(CURDIR)/BlockEditor/dataplot.py $(BINDIR)/dataplot
	ln -s $(CURDIR)/BlockEditor/xblk2Blk.py $(BINDIR)/xblk2Blk
	ln -s $(CURDIR)/DriverNRT/loadnrt $(BINDIR)/loadnrt

user:
	echo $(PYCTL) >> ~/.bashrc
	echo 'export PYEDITOR=emacs' >> ~/.bashrc
	echo 'export PYTHONPATH=$(HOME)/Documents/PYTHON:$(CURDIR)/resources/blocks/rcpBlk' >> ~/.bashrc

alias:
	echo 'alias pysimCoder='"'$(CURDIR)/pysim-run.sh'"''  >> ~/.bashrc
	echo 'alias psc='"'pysimCoder'"''  >> ~/.bashrc

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
	cd CodeGen/Raspberry_PI/lib; \
	wget robertobucher.dti.supsi.ch/wp-content/uploads/2017/03/RaspLibs.zip; \
	unzip RaspLibs.zip; rm *.zip




