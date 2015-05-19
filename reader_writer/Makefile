##########################################################
# PolySync API Makefile
##########################################################

TARGET  := polysync-api-reader
SRCS    :=  src/polysync_reader.c
OBJS    := ${SRCS:.c=.o}
DEPS    := ${SRCS:.c=.dep}
XDEPS   := $(wildcard ${DEPS})



#
CC	= gcc

#
include ${PSYNC_HOME}/build_res.mk


.PHONY: all clean api_writer install uninstall
all: ${TARGET} api_writer

ifneq (${XDEPS},)
include ${XDEPS}
endif



#
${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^ ${INCLUDE} ${LIBS}
	-mkdir -p bin
	-mv ${TARGET} ./bin/${TARGET}


#
${OBJS}: %.o: %.c %.dep
	${CC} ${INCLUDE} ${CCFLAGS} -o $@ -c $<


#
${DEPS}: %.dep: %.c Makefile
	${CC} ${INCLUDE} ${CCFLAGS} -MM $< > $@


#
api_writer::
	${CC} -o bin/polysync-api-writer src/polysync_writer.c ${INCLUDE} ${LIBS}


#
clean::
	-rm -f src/*.o src/*.dep bin/polysync-api-writer bin/${TARGET}

#
install: ${TARGET} api_writer
	-cp bin/${TARGET} ${PSYNC_HOME}/bin/${TARGET}
	-cp bin/polysync-api-writer ${PSYNC_HOME}/bin/polysync-api-writer

#
uninstall::
	-rm -f ${PSYNC_HOME}/bin/${TARGET}
	-rm -f ${PSYNC_HOME}/bin/polysync-api-writer
