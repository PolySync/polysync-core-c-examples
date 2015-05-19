##########################################################
#
##########################################################

#
TARGET=analytics-simple

#
PLOT_SCRIPT=plot-analytics

#
CC	= gcc

#
include ${PSYNC_HOME}/build_res.mk


#
.PHONY: all clean analytics_simple install uninstall
all: analytics_simple



#
analytics_simple::
	-chmod +x bin/${PLOT_SCRIPT}
	${CC} ${CCFLAGS} ${INCLUDE} src/analytics_simple.c -o bin/${TARGET} ${LIBS}


#
clean::
	-rm -f src/*.o src/*.dep
	-rm -f bin/${TARGET}
	-rm -f ospl-*.log


#
install: analytics_simple
	-cp bin/${TARGET} ${PSYNC_HOME}/bin/${TARGET}
	-cp bin/${PLOT_SCRIPT} ${PSYNC_HOME}/bin/${PLOT_SCRIPT}


#
uninstall::
	-rm -f ${PSYNC_HOME}/bin/${TARGET}
	-rm -f ${PSYNC_HOME}/bin/${PLOT_SCRIPT}
