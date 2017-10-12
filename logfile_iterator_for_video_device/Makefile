##########################################################
# makefile for logfile-iterator-for-video-device
##########################################################


# source PolySync environment if not already done, assumes x86_64 if set here
# usually, the environment has these set
PSYNC_HOME ?= /usr/local/polysync
OSPL_HOME ?= $(PSYNC_HOME)/utils/x86_64.linux

# binary target
BIN_TARGET := bin/polysync-logfile-iterator-for-video-device-c

# static library target
LIB_TARGET := libpolysync_image_data_logfile_utils.a

# Example's header disticnt from external dependency headers
UTILS_HEADER := include/video_log_utils.h

# sources
SRCS := src/main.c src/video_log_utils.c

# object files, dep files
UTILS_OBJ := src/video_log_utils.o
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.dep)
XDEPS := $(wildcard $(DEPS))

# get standard PolySync build resources
include $(PSYNC_HOME)/build_res.mk

INCLUDE += -Iinclude

# compiler
CC = gcc

# add data model library
LIBS += -lpolysync_data_model -lpopt lib/libuvc.a -lpthread -lusb-1.0

all: dirs $(BIN_TARGET)

ifneq ($(XDEPS),)
include $(XDEPS)
endif

# directories
dirs::
	mkdir -p bin

$(BIN_TARGET): $(OBJS) $(UTILS_HEADER)
	$(CC) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $<

static_lib: $(UTILS_OBJ) $(UTILS_HEADER)
	cd lib && ar -x libuvc.a && cd ..
	ar rcs $(LIB_TARGET) $^ lib/*.o

$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDE) -MM $< > $@

clean:
	-rm -f src/*.o
	-rm -f lib/*c.o
	-rm -f src/*.dep
	-rm -f $(BIN_TARGET)
	-rm -f $(LIB_TARGET)
	-rm -f bin/*
	-rm -rf ospl-*.log
