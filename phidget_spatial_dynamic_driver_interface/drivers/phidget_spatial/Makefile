##########################################################
# Makefile for Phidget Spatial Hardware Driver
##########################################################


# source $(PSYNC_HOME) if not already done, assumes x86_64
# usually, the environment has these set
PSYNC_HOME ?= /usr/local/polysync
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

# target, static library file
TARGET  := $(LIB_DIR)/libps_phidget_spatial_driver.a

# source files
SRCS	:=  src/phidget_spatial_driver.c

# test interface
TEST_INTERFACE_TARGET := $(BUILD_DIR)/bin/test-phidget-spatial-interface

# dep files
OBJS    := $(SRCS:.c=.o)
DEPS    := $(SRCS:.c=.dep)
XDEPS   := $(wildcard $(DEPS))

# get standard PolySync build resources
include $(PSYNC_HOME)/build_res.mk

# compiler
CC = gcc

# include directories
INCLUDE += -Iinclude

# add PIC
CCFLAGS += -fPIC

# all builds directories and target
#all: dirs $(TARGET) tests
all: dirs $(TARGET)

# check deps
ifneq ($(XDEPS),)
include $(XDEPS)
endif

# target and md5 file
$(TARGET): $(OBJS)
	ar rcs $@ $^
	cd $(dir $(TARGET)) && md5sum $(notdir $@) > $(notdir $(TARGET)).md5

# object files
$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $<

# dep files
$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDE) -MM $< > $@

# directories
dirs::
	mkdir -p $(BUILD_DIR)/bin
	mkdir -p $(LIB_DIR)

# install to system
install: all
	cp $(TARGET) $(PSYNC_HOME)/lib/

# tests
test:: dirs
	$(CC) $(CCFLAGS) $(INCLUDE) -I$(BUILD_HOME)/tests/tools -o $(TEST_INTERFACE_TARGET) src/phidget_spatial_test_interface.c $(TARGET) $(LIBS) -lphidget21

# clean
clean::
	rm -f src/*.o
	rm -f src/*.dep
	rm -f ospl-*.log
	rm -f $(TEST_INTERFACE_TARGET)

# dist-clean
dist-clean:: clean
	rm -f $(TARGET)
	rm -f $(TARGET).md5
