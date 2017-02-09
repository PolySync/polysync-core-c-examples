##########################################################
# Makefile for Phidget Spatial PolySync Interface
##########################################################


# source PS environment if not already done, assumes x86_64 if set here
# usually, the environment has these set
PSYNC_HOME ?= /usr/local/polysync
OSPL_HOME ?= $(PSYNC_HOME)/utils/x86_64.linux
PSYNC_ARCH ?= x86_64
PSYNC_OS ?= ubuntu_14_04

# if build directory wasn't specified, use assumed path
BUILD_DIR ?= ../..

# if build home directory wasn't specified, use assumed path
# build home is used to provide the testing framework (tests/tools/unit_test.h)
# add -I$(BUILD_HOME)/tests/tools to your test compile INCLUDES to get it
BUILD_HOME ?= $(shell cd $(BUILD_DIR)/../../ && pwd)

# this arch/OS library directory
LIB_DIR := $(BUILD_DIR)/lib/$(PSYNC_ARCH)/$(PSYNC_OS)

# targets, shared library files
TARGET  := $(LIB_DIR)/libps_phidget_spatial_interface.so

# source files
SRCS    :=  src/connect.c \
            src/debug.c \
            src/diagnostics.c \
            src/getset.c \
            src/interface.c \
            src/log.c \
            src/options.c \
            src/process.c \
            src/read.c \
            src/sdf.c \
            src/type_support.c \

# dep files
OBJS    := $(SRCS:.c=.o)
DEPS    := $(SRCS:.c=.dep)
XDEPS   := $(wildcard $(DEPS))

# get standard PolySync build resources
include $(PSYNC_HOME)/build_res.mk

# compiler
CC = gcc

# silences non-error compile output so all you see is test results
SILENT_CC = @gcc

# include directories
INCLUDE += -Iinclude -I$(BUILD_DIR)/include -I$(BUILD_DIR)/drivers/phidget_spatial/include

# add PIC
CCFLAGS += -fPIC

# no undefined symbols
CCFLAGS += -Wl,--no-undefined

# shared lib symbol export mapping
# only expose the PAL symbols expected by the dynamic loader
SHARED_LIB_EXPORT_MAP = "{\nglobal: psync_interface_*;\nlocal: *;\n};"
SHARED_LIB_EXPORTS = -fvisibility=hidden -fvisibility-inlines-hidden -s -Wl,--version-script=exportmap

# base PS libs
SHARED_LIBS = $(PSLIBS)

# needs data model symbols
SHARED_LIBS += -lpolysync_data_model

# add util lib
SHARED_LIBS += -L$(LIB_DIR) -lps_interface_util

# add HAL driver
SHARED_LIBS += $(LIB_DIR)/libps_phidget_spatial_driver.a

# add PS libs
SHARED_LIBS += $(DEPLIBS)

# all builds directories and target
all: dirs $(TARGET)

# check deps
ifneq ($(XDEPS),)
include $(XDEPS)
endif

# target
$(TARGET): $(OBJS)
	@echo $(SHARED_LIB_EXPORT_MAP) > exportmap
	$(CC) -shared $(CCFLAGS) -o $@ $^ -ldl $(SHARED_LIBS) $(SHARED_LIB_EXPORTS) -lphidget21
	cd $(dir $(TARGET)) && md5sum $(notdir $@) > $(notdir $(TARGET)).md5

# test target
$(TEST_TARGET): $(TEST_PAL_OBJS)
	$(SILENT_CC) -shared $(CCFLAGS) -o $@ $^ -ldl $(SHARED_LIBS) -lphidget21

# test enabled versions of PAL object files
$(TEST_PAL_OBJS): test/%.o: src/%.c
	$(SILENT_CC) $(CCFLAGS) -DTEST_TARGET $(INCLUDE) -o $@ -c $<

# test object files
$(TEST_OBJS): %.o: %.c
	$(SILENT_CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $<

# build shared objects for test files and run tests on each
$(TEST_LIBS):: %.so: %.o $(TEST_TARGET)
	$(SILENT_CC) $(CCFLAGS) $(INCLUDE) -shared -o $@ $< $(TEST_TARGET) $(LIBS) -lcgreen
	cgreen-runner --colours $@

test: $(TEST_LIBS)

# object files
$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $<

# dep files
$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDE) -MM $< > $@

# directories
dirs::
	mkdir -p $(LIB_DIR)

# clean
clean::
	rm -f src/*.o
	rm -f src/*.dep
	rm -f test/*.so
	rm -f test/*.o
	rm -f exportmap
	rm -f ospl-*.log

# dist-clean
dist-clean:: clean
	rm -f $(TARGET)
	rm -f $(TARGET).md5
