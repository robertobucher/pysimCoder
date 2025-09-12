MODEL ?=
ADDITIONAL_DEFINES ?=
ADD_FILES ?=

all: default

.PHONY: all default

# Enable or define during make invocation to embed content of the specified
# directory into ROM filesystem mounted into /etc on the target
#EMBEDROMFS = romfs

SHV_INC = $(PYCODEGEN)/Common/shv/include
GENERATED_INC = $(PYCODEGEN)/nuttx/include-generated

RM = rm -f
FILES_TO_CLEAN = *.o $(MODEL) $(MODEL).elf $(MAIN)-builtintab.c

MODEL_BINARIES := ../$(MODEL).bin ../$(MODEL)

ifndef NUTTX_EXPORT
$(warning Specify NUTTX_EXPORT)
$(warning make_rtw NUTTX_EXPORT=/path/to/nuttx-export)
$(error NUTTX_EXPORT not defined, cannot continue)
endif

ifneq ($(wildcard $(NUTTX_EXPORT)/registry/*.bdat),)
NUTTX_REGISTER_BINARIES ?= y
endif

ifneq ($(shell grep 'CONFIG_INTELHEX_BINARY[[:blank:]]*1' "$(NUTTX_EXPORT)/include/nuttx/config.h"),)
CONFIG_INTELHEX_BINARY = 1
MODEL_BINARIES += ../$(MODEL).hex
endif

ifneq ($(wildcard $(NUTTX_EXPORT)/tools/nximage.py),)
NXIMAGE=$(NUTTX_EXPORT)/tools/nximage.py
NXIMAGE_FILE=../$(MODEL).nximg
MODEL_BINARIES += $(NXIMAGE_FILE)
endif

include $(NUTTX_EXPORT)/scripts/Make.defs

NUTTX_INCLUDES = -isystem $(NUTTX_EXPORT)/include

ifneq ($(filter -Wl%, $(NXFLATLDFLAGS1)$(NXFLATLDFLAGS2)),)
GCC_LD_OPTION = -Wl,
LD_RAW ?= $(CROSSDEV)ld
else
LD_RAW ?= $(LD)
endif

ifeq ($(LDSTARTGROUP),)
LDSTARTGROUP = $(GCC_LD_OPTION)--start-group
LDENDGROUP = $(GCC_LD_OPTION)--end-group
endif

SYSTEM_LIBS += $(LDSTARTGROUP) $(LDLIBS) $(EXTRA_LIBS) $(LDENDGROUP)
ELF_MODULE_LIBS = $(LDSTARTGROUP) $(EXTRA_LIBS) $(LDENDGROUP)

LDFLAGS += -L $(NUTTX_EXPORT)/libs

ifneq ($(shell $(CC) --target-help | grep -e '-mlong-calls\b'),)
TARGET_ARCH_LONG_CALLS = -mlong-calls
endif

TARGET_ARCH_FLAGS ?= $(ARCHCFLAGS) $(ARCHCPUFLAGS) \
	$(TARGET_ARCH_LONG_CALLS) -fno-common -DWITHOUT_MLOCK

DEFAULT_OPT_OPTS ?= $(ARCHOPTIMIZATION)

ifndef OPT_OPTS
OPT_OPTS = $(DEFAULT_OPT_OPTS)
endif

LD_SCRIPTS = $(LDNAME:%=$(NUTTX_EXPORT)/scripts/%)

LDFLAGS_LD_SCRIPTS += $(LD_SCRIPTS:%=-T %)

LDFLAGS  += --entry=__start

ifndef LDELFFLAGS
ELF_FILE_LDSCRIPT ?= $(wildcard $(NUTTX_EXPORT)/scripts/gnu-elf.ld)
ifneq ($(ELF_FILE_LDSCRIPT),)
LDELFFLAGS = $(LDFLAGS) -r -e main -T "$(ELF_FILE_LDSCRIPT)"
endif
endif

ifneq ($(shell grep 'CONFIG_ELF[[:blank:]]*1' "$(NUTTX_EXPORT)/include/nuttx/config.h"),)
CONFIG_ELF_BINARY = 1
MODEL_BINARIES := ../$(MODEL).elf $(MODEL_BINARIES)
endif

MAIN ?= nuttx_main

ifneq ($(EMBEDROMFS),)
ifeq ($(shell grep CONFIG_ETC_ROMFSMOUNTPT "$(NUTTX_EXPORT)/include/nuttx/config.h"),)
$(error CONFIG_ETC_ROMFS and CONFIG_ETC_ROMFSMOUNTPT are required for EMBEDROMFS)
endif
ifeq ($(wildcard $(abspath $(EMBEDROMFS))),)
EMBEDROMFS_ABS=$(abspath ../$(EMBEDROMFS))
ifeq ($(wildcard $(EMBEDROMFS_ABS)),)
$(error Specified EMBEDROMFS=$(EMBEDROMFS) is not found)
endif
else
EMBEDROMFS_ABS=$(abspath $(EMBEDROMFS))
endif
romfs_img_len_encode.c: Makefile
	echo >$@ \
	"#include <stdint.h>\n\
	extern uint8_t romfs_img_size[];\
	const unsigned int romfs_img_len = (uintptr_t)romfs_img_size;"
romfs_img: $(shell find "$(EMBEDROMFS_ABS)" -type f,d) Makefile
	rm -f $@.tmp
	genromfs -f romfs_img.tmp -V romfs -d $<
	if ( [ ! -e $@ ] || ! cmp -s $@.tmp $@ ) ; then cp $@.tmp $@ ; fi
romfs_img.o: romfs_img
	rm -f romfs_img.o.tmp
	$(LD_RAW) -r --accept-unknown-input-arch -b binary \
	$(filter -m%,$(LDFLAGS:$(GCC_LD_OPTION)%=%)) \
	--defsym=romfs_img_size=_binary_romfs_img_size \
	--defsym=romfs_img=_binary_romfs_img_start \
	--defsym=romfs_img_start=_binary_romfs_img_start \
	--defsym=romfs_img_end=_binary_romfs_img_end \
	-o $@.tmp $<
	$(OBJCOPY) --rename-section .data=.rodata,alloc,load,readonly,data,contents \
	$@.tmp $@
ADD_FILES += romfs_img.o romfs_img_len_encode.o
endif

OBJSSTAN = $(MAIN).o $(MODEL).o $(ADD_FILES)

ifeq ($(NUTTX_REGISTER_BINARIES),y)
BIN_ENTRIES += main
OBJSSYSOVERRIDE += $(MAIN)-builtintab.o

$(MAIN)-builtintab.o : $(MAIN)-builtintab.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(MAIN)-builtintab.c : $(wildcard $(NUTTX_EXPORT)/registry/*.bdat) $(wildcard $(NUTTX_EXPORT)/registry/*.pdat)
	@cat >$@ $(wildcard $(NUTTX_EXPORT)/registry/*.pdat)
	for ent in $(BIN_ENTRIES) ; do \
		echo >>$@ 'int '$$ent'(int argc, char *argv[]);' \
	; done
	@echo >>$@ '#include <nuttx/config.h>'
	@echo >>$@ '#include <nuttx/lib/builtin.h>'
	@echo >>$@ 'const struct builtin_s g_builtins[] = {'
	@cat >>$@ $(wildcard $(NUTTX_EXPORT)/registry/*.bdat)
	for ent in $(BIN_ENTRIES) ; do \
		echo >>$@ '{ "'$$ent'", SCHED_PRIORITY_DEFAULT, CONFIG_DEFAULT_TASK_STACKSIZE + 1024, '$$ent'},' ; \
	done
	@echo >>$@ "{ NULL, 0, 0, 0 }};"
	@echo >>$@ "const int g_builtin_count = sizeof(g_builtins) / sizeof(g_builtins[0]);"
endif

INCLUDES += -I$(INCDIR) -I$(COMMON_INCDIR) $(NUTTX_INCLUDES)

CFLAGS = $(TARGET_ARCH_FLAGS) $(ARCHWARNINGS) $(OPT_OPTS) $(INCLUDES) -DMODEL=$(MODEL) $(ADDITIONAL_DEFINES)

# Test, whether the directory exists
ifneq ($(wildcard $(GENERATED_INC)),)
CFLAGS += -I$(GENERATED_INC)
endif
CFLAGS += -I$(SHV_INC)

CXXFLAGS = $(TARGET_ARCH_FLAGS) $(ARCHCXXFLAGS) $(ARCHWARNINGSXX) $(OPT_OPTS) $(INCLUDES) -DMODEL=$(MODEL)

LIB = $(LIBDIR)/libpyblk.a
LIB = $(LIBDIR)/libpyblk.a

# shv-libs4c has zlib as a dependancy
LIB += $(wildcard $(LIBDIR)/libshvtree.a)
LIB += $(wildcard $(LIBDIR)/libshvchainpack.a)
LIB += $(wildcard $(LIBDIR)/libulut.a)

default: $(MODEL_BINARIES)

$(MAIN).c: $(MAINDIR)/$(MAIN).c $(MODEL).c
	@echo "CP: $< -> $@"
	@cp $< .

%.o: ../%.c
	@echo "CC: $$(basename $<)"
	@$(CC) -c -o $@ $(CFLAGS) $<

../$(MODEL).elf: $(OBJSSTAN) $(LIB)
	[ -z "$(LDELFFLAGS)" ] || \
	$(LD) -L $(NUTTX_EXPORT)/scripts $(LDELFFLAGS) \
	  $(ADDITIONAL_LDFLAGS) -o $@ \
	  $(GCC_LD_OPTION)-Map $(GCC_LD_OPTION)$(notdir $@).map \
	  $(OBJSSTAN) $(LIB) $(LIBS) $(ELF_MODULE_LIBS)
	@echo "### Created ELF loadable file: $(MODEL).elf"

../$(MODEL): $(OBJSSTAN) $(OBJSSYSOVERRIDE) $(LIB)
	$(LD) -L $(NUTTX_EXPORT)/scripts $(LDFLAGS) \
	  $(ADDITIONAL_LDFLAGS) $(LDFLAGS_LD_SCRIPTS) \
	  -o $@ $(HEAD_OBJ:%=$(NUTTX_EXPORT)/startup/%) \
	  $(OBJSSTAN) $(OBJSSYSOVERRIDE) $(LIB) $(SYSTEM_LIBS)
	@echo "### Created executable: $(MODEL)"

../$(MODEL).hex : ../$(MODEL)
	@echo "OBJCOPY: $(MODEL).hex"
	@$(OBJCOPY) $(OBJCOPYARGS) -O ihex $< $@

../$(MODEL).bin : ../$(MODEL)
	@echo "OBJCOPY: $(MODEL).bin"
	@$(OBJCOPY) $(OBJCOPYARGS) -O binary $< $@

ifneq ($(NXIMAGE),)
$(NXIMAGE_FILE) : ../$(MODEL).bin
	@echo "GENERATING NXBoot Image"
	@$(NXIMAGE) --header_size=0x200 --identifier=0 \
	    --version=0.0.1 $< $@
endif

clean:
	@$(RM) $(FILES_TO_CLEAN)
