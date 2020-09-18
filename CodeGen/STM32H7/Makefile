all: devices objlib objects OTG

.PHONY: devices objlib objects OTG

devices:
	cd devices; make; make install; make clean

objlib:
	cd objects/srcHAL; make; make install; make clean

objects:
	cd objects/src; make; make clean

OTG: 
	cd OTG; make; make clean
	cd OTG/obj; make install; make clean


