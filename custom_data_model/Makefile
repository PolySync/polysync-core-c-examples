##########################################################
#
##########################################################

#
CC	= gcc

#
include ${PSYNC_HOME}/build_res.mk

#
INCLUDE	+= -Iinclude -Iudm


.PHONY: all clean
all: udm udm_reader udm_writer logfile udm_logfile

udm::
	mkdir -p bin
	${PSYNC_HOME}/bin/udm-gen -i idl/my_dm.idl

udm_reader::
	${CC} ${CCFLAGS} ${INCLUDE} src/udm_reader.c -o bin/udm-reader ${LIBS}

udm_writer::
	${CC} ${CCFLAGS} ${INCLUDE} src/udm_writer.c -o bin/udm-writer ${LIBS} udm/libudm.so

logfile::
	${CC} ${CCFLAGS} ${INCLUDE} src/load_logfile.c -o bin/load-logfile ${LIBS}
	${CC} ${CCFLAGS} ${INCLUDE} src/write_logfile.c -o bin/write-logfile ${LIBS}

udm_logfile::
	${CC} ${CCFLAGS} -DDO_MY_DM_TEST ${INCLUDE} src/load_logfile.c -o bin/udm-load-logfile ${LIBS}
	${CC} ${CCFLAGS} -DDO_MY_DM_TEST ${INCLUDE} src/write_logfile.c -o bin/udm-write-logfile ${LIBS} udm/libudm.so

clean::
	-rm -f *~ *.o src/*.o src/*.dep bin/*
	-rm -rf udm
	-rm -f ospl-*.log
