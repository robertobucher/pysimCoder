all: addfiles modules fmu full_lib driver link
reduced: addfiles modules fmu lib driver link

BINDIR = /usr/local/bin
CURDIR = $(shell pwd)
CWD = $(shell pwd)
PYCTL = export PYSUPSICTRL=$(CWD)

addfiles:
	git clone https://github.com/python-control/python-control.git
	git clone https://github.com/python-control/Slycot

	cd python-control; python setup.py install
	cd Slycot; python setup.py install
	rm -rf python-control Slycot

modules:
	cd toolbox/supsictrl; python setup.py install
	cd toolbox/supsisim; python setup.py install

fmu:
	cd CodeGen/fmu; make all; make install

lib:
	cd CodeGen/devices; make reduced

full_lib:
	cd CodeGen/devices; make all

link:
	cd $(BINDIR); rm -f pysimCoder pyParams gen_pydev loadnrt defBlocks dataplot xblk2Blk
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

RaspLib:
	cd CodeGen/RPI_devices; make; make install
	cd CodeGen/lib_Pi; wget get robertobucher.dti.supsi.ch/wp-content/uploads/2017/03/RaspLibs.zip; unzip RaspLibs.zip; rm *.zip

user:
	echo $(PYCTL) >> ~/.bashrc
	echo 'export PYEDITOR=emacs' >> ~/.bashrc
	echo 'export PYTHONPATH=$(HOME)/Documents/PYTHON:$(CURDIR)/resources/blocks/rcpBlk' >> ~/.bashrc

script:
	echo $(PYCTL) > pysimCoder
	echo 'export PYEDITOR=emacs' >> pysimCoder
	echo 'export PYTHONPATH=$(CURDIR)/resources/blocks/rcpBlk' >> pysimCoder
	cat launch.txt >> pysimCoder
	chmod a+x pysimCoder






