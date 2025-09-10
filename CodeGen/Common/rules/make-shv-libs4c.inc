# Makefile fragment for devices to download and build shv-libs4c library

.PHONY: make-shv-libs4c

make-shv-libs4c: make-ulut
	$(EXT_LIBS)/extlib-fetch.sh shv-libs4c
	cd $(EXT_LIBS)/shv-libs4c && make default-config \
		CONFIG_SHV_LIBS4C_PLATFORM=$(CONFIG_SHV_LIBS4C_PLATFORM) \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="$(EXTLIB_CFLAGS)" \
		CXXFLAGS="$(EXTLIB_CXXFLAGS)" \
		LD="$(LD)" \
		LDFLAGS="$(EXTLIB_LDFLAGS)" \
		CONFIG_FILE=$(CURDIR)/config-shv-libs4c.omk \
		W=1 \
		OUTPUT_DIR="$(CURDIR)" \
		USER_INCLUDE_DIR="$(OMK_COMPILED_INC_PATH)" \
		USER_LIB_DIR="$(OMK_COMPILED_LIB_PATH)"
	cd $(EXT_LIBS)/shv-libs4c && make all \
		CONFIG_SHV_LIBS4C_PLATFORM=$(CONFIG_SHV_LIBS4C_PLATFORM) \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="$(EXTLIB_CFLAGS)" \
		CXXFLAGS="$(EXTLIB_CXXFLAGS)" \
		LD="$(LD)" \
		LDFLAGS="$(EXTLIB_LDFLAGS)" \
		CONFIG_FILE=$(CURDIR)/config-shv-libs4c.omk \
		W=1 \
		OUTPUT_DIR="$(CURDIR)" \
		USER_INCLUDE_DIR="$(OMK_COMPILED_INC_PATH)" \
		USER_LIB_DIR="$(OMK_COMPILED_LIB_PATH)"
