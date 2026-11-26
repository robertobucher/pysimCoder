
# Paths
PROJECT_DIR := $(PYSUPSICTRL)
UNITY_DIR   := $(CURDIR)/Unity/src
TEST_DIR    := $(CURDIR)/tests
BUILD_DIR   := $(CURDIR)/build

CC := gcc
CFLAGS := -Wall -Wextra -Werror -g 

# Header directories
INCLUDE_DIRS :=  $(UNITY_DIR) \
		$(PROJECT_DIR)/CodeGen/Common/include/

INCLUDES := $(addprefix -I,$(INCLUDE_DIRS))
HEADERS := $(wildcard $(addsuffix /*.h,$(INCLUDE_DIRS)))

# Source needed files for test
# $(PROJECT_DIR)/CodeGen/Common/can/can_data.c
SRC := 

UNITY_SRC := $(UNITY_DIR)/unity.c # Unity framework

# Require Unity sources; allow cleaning without Unity installed.
ifneq ($(MAKECMDGOALS),clean)
UNITY_REQUIRED := $(UNITY_DIR)/unity.c $(UNITY_DIR)/unity.h $(UNITY_DIR)/unity_internals.h
UNITY_MISSING := $(filter-out $(wildcard $(UNITY_REQUIRED)),$(UNITY_REQUIRED))
ifneq ($(strip $(UNITY_MISSING)),)
$(error Unity directory is missing, empty, or incomplete: $(UNITY_DIR). Init Unity git submodule before running tests.)
endif
endif

# Skip test files by name: make test EXCLUDE_TESTS="test_can_data.c test_other.c"
EXCLUDE_TESTS ?= test_can_data.c

# Test run files
TEST_SRC := $(filter-out $(addprefix $(TEST_DIR)/,$(EXCLUDE_TESTS)),$(wildcard $(TEST_DIR)/test_*.c))

# One executable per test file
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%,$(TEST_SRC))

.PHONY: all test clean

all: test

# Build each test executable
$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(SRC) $(UNITY_SRC) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(filter %.c,$^) -o $@

test: $(TEST_BINS)
	@status=0; \
	for test_bin in $(TEST_BINS); do \
	    echo "Running $$test_bin"; \
	    "$$test_bin" || status=1; \
	done; \
	exit $$status

# Remove generated files
clean:
	rm -rf $(BUILD_DIR)
