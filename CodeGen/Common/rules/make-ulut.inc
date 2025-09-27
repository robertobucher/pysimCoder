# Makefile fragment for devices to download and build uLUt library

.PHONY: make-ulut

make-ulut:
	$(EXT_LIBS)/extlib-fetch.sh ulut
	cd $(EXT_LIBS)/ulut && make all \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="$(EXTLIB_CFLAGS)" \
		CXXFLAGS="$(EXTLIB_CXXFLAGS)" \
		LD="$(LD)" \
		LDFLAGS="$(EXTLIB_LDFLAGS)" \
		CONFIG_OC_ULUT_TESTS=n \
		CONFIG_OC_ULUT_INCDIRONLY=y \
		CONFIG_FILE=$(CURDIR)/config-ulut.omk \
		W=1 \
		OUTPUT_DIR="$(CURDIR)" \
		USER_INCLUDE_DIR="$(OMK_COMPILED_INC_PATH)" \
		USER_LIB_DIR="$(OMK_COMPILED_LIB_PATH)"
