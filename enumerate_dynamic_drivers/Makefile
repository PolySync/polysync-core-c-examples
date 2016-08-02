##########################################################
# makefile for enumerate dynamic drivers example
##########################################################


# source PolySync environment if not already done, assumes x86_64 if set here
# usually, the environment has these set
PSYNC_HOME ?= /usr/local/polysync
OSPL_HOME ?= $(PSYNC_HOME)/utils/x86_64.linux

# target
TARGET	:= bin/polysync-enumerate-dynamic-drivers

# sources
SRCS    :=  src/enumerate_dynamic_drivers.c

# object files, dep files
OBJS    := $(SRCS:.c=.o)
DEPS    := $(SRCS:.c=.dep)
XDEPS   := $(wildcard $(DEPS))

# get standard PolySync build resources
include $(PSYNC_HOME)/build_res.mk

# compiler
CC = gcc

# add node template library, must be first
LIBS += -lpolysync_data_model

# add dynamic driver includes
INCLUDE += -I$(PSYNC_HOME)/dynamic_driver/include

#
all: dirs $(TARGET)

#
ifneq ($(XDEPS),)
include $(XDEPS)
endif

# directories
dirs::
	mkdir -p bin

#
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

#
$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $<

#
$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDE) -MM $< > $@

# install to system
install: all
	cp $(TARGET) $(PSYNC_HOME)/bin/

#
clean:
	-rm -f src/*.o
	-rm -f src/*.dep
	-rm -f $(TARGET)
	-rm -f bin/*
	-rm -rf ospl-*.log
