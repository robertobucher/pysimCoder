all: install

install: 
	cd STM32_USB_Device_Library/Core/Src; make
	cd STM32_USB_Device_Library/Class/CDC/Src; make
	cd USB_DEVICE/Target; make
	cd USB_DEVICE/App; make

clean:
	cd STM32_USB_Device_Library/Core/Src; make clean
	cd STM32_USB_Device_Library/Class/CDC/Src; make clean
	cd USB_DEVICE/Target; make clean
	cd USB_DEVICE/App; make clean




