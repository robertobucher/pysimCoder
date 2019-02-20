all: addfiles modules full_lib driver link

BINDIR = /usr/local/bin
CURDIR = $(shell pwd)
CWD = $(shell pwd)
PYCTL = export PYSUPSICTRL=$(CWD)

addfiles:
	wget https://codeload.github.com/python-control/python-control/zip/master -O python-control-master.zip
	wget https://codeload.github.com/python-control/Slycot/zip/master -O Slycot-master.zip

	unzip python-control-master.zip
	unzip Slycot-master.zip
	cd python-control-master; python setup.py install
	cd Slycot-master; python setup.py install
	rm -rf python-control-master Slycot-master python-control-master.zip Slycot-master.zip 

modules:
	cd toolbox/supsictrl; python setup.py install
	cd toolbox/supsisim; python setup.py install
	cd toolbox/PyQT; python setup.py install

lib:
	cd CodeGen; rm -rf lib; mkdir lib
	cd CodeGen/devices; make reduced

full_lib:
	cd CodeGen; rm -rf lib; mkdir lib
	cd CodeGen/devices; make all

link:
	cd $(BINDIR); rm -f pysimCoder pyParams gen_pydev loadnrt defBlocks dataplot xblk2Blk
	cp BlockEditor/gen_pydev $(BINDIR)/
	cd $(CURDIR)/BlockEditor; chmod a+x *.py
	cd $(CURDIR)/DriverNRT/; chmod a+x loadnrt
	ln -s $(CURDIR)/BlockEditor/pysimCoder.py $(BINDIR)/pysimCoder
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
	echo 'export PYTHONPATH=$(HOME)/Documents/PYTHON:$(CURDIR)/resources/blocks/blocks/rcpBlk' >> ~/.bashrc
	echo 'export PYUSERBLKS=$(HOME)/Documents/PYTHON/rcpBlk' >> ~/.bashrc
	mkdir -p $(HOME)/Documents/PYTHON/rcpBlk/lib
	cp ./userLib/* $(HOME)/Documents/PYTHON/rcpBlk/lib



