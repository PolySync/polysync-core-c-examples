##########################################################
# makefile for viewer-lite
##########################################################

# target
TARGET	:= bin/viewer-lite

# sources
SRCS    :=  src/ps_interface.c \
	    src/color.c \
	    src/ruler.c \
	    src/render.c \
	    src/ground_plane.c \
	    src/origin_model.c \
	    src/grid.c \
	    src/entity_manager.c \
	    src/gui.c \
	    src/main.c

# object files, dep files
OBJS    := $(SRCS:.c=.o)
DEPS    := $(SRCS:.c=.dep)
XDEPS   := $(wildcard $(DEPS))

# PolySync, defaults to x86_64.linux
PSYNC_HOME ?= /usr/local/polysync
OSPL_HOME ?= $(PSYNC_HOME)/util/x86_64.linux
include $(PSYNC_HOME)/build_res.mk

# compiler
CC = gcc

# base target compiler flags, provided by build_res.mk for now
#CCFLAGS += -Wall -std=gnu99
#CCFLAGS += -Wextra \
#          -Wformat=2 -Wno-unused-parameter -Wshadow \
#          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
#          -Wnested-externs -Wmissing-include-dirs

# include GL EXT
CCFLAGS += -DGL_GLEXT_PROTOTYPES -DGLX_GLXEXT_PROTOTYPES

# includes
INCLUDE	+= -Iinclude

# libraries
LIBS	+= -lglut -lGLU -lGL -lX11 -lm

#
all: dirs $(TARGET)

#
ifneq ($(XDEPS),)
include $(XDEPS)
endif

# directories
dirs::
	mkdir -p doc/html
	mkdir -p doc/latex
	mkdir -p doc/pdf
	mkdir -p bin

#
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

#
$(OBJS): %.o: %.c %.dep
	$(CC) $(INCLUDE) $(CCFLAGS) -o $@ -c $<

#
$(DEPS): %.dep: %.c Makefile
	$(CC) $(INCLUDE) $(CCFLAGS) -MM $< > $@

#
docs: dirs
	cd doc && doxygen Doxyfile;
	cd doc/latex && make clean && make && cp refman.pdf ../pdf/refman.pdf

#
point-gen:
	$(CC) $(INCLUDE) $(CCFLAGS) -o bin/$@ tools/point_gen.c $(LIBS)

#
obj-gen:
	$(CC) $(INCLUDE) $(CCFLAGS) -o bin/$@ tools/obj_gen.c $(LIBS)

#
track-gen:
	$(CC) $(INCLUDE) $(CCFLAGS) -o bin/$@ tools/track_gen.c $(LIBS)

#
clean:
	-rm -f src/*.o
	-rm -f src/*.dep
	-rm -f $(TARGET)
	-rm -f bin/*
	-rm -rf doc/html
	-rm -rf doc/latex
	-rm -rf doc/pdf
	-rm -rf ospl-*.log
