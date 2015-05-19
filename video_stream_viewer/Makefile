##########################################################
#
##########################################################

TARGET  := video-stream-viewer
SRCS    := src/vsv.c
OBJS    := ${SRCS:.c=.o}
DEPS    := ${SRCS:.c=.dep}
XDEPS   := $(wildcard ${DEPS})

#
CC      = gcc

#
include ${PSYNC_HOME}/build_res.mk

#
LIBS	+=  -lSDL -lSDL_gfx

#
INCLUDE	+=  -Iinclude -I/usr/include/SDL


.PHONY: all clean install uninstall
all: ${TARGET}

ifneq (${XDEPS},)
include ${XDEPS}
endif

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^ ${INCLUDE} ${LIBS}
	-mkdir -p bin
	-mv ${TARGET} bin/${TARGET}

${OBJS}: %.o: %.c %.dep
	${CC} ${INCLUDE} ${CCFLAGS} -o $@ -c $<

${DEPS}: %.dep: %.c Makefile
	${CC} ${INCLUDE} ${CCFLAGS} -MM $< > $@

clean::
	-rm -f src/*.o src/*.dep bin/${TARGET}
	-rm -f ospl-*.log

install: ${TARGET}
	-cp bin/${TARGET} ${PSYNC_HOME}/bin/${TARGET}

uninstall::
	-rm -f ${PSYNC_HOME}/bin/${TARGET}