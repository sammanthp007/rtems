ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

EXTRA_DIST = README

noinst_PROGRAMS =


include_bsp_HEADERS = include/irq.h \
	../../shared/include/irq-generic.h \
	../../shared/include/irq-info.h \
	../../shared/include/bootcard.h \
	../../shared/include/utility.h \
	../shared/include/start.h \
	../shared/include/tictac.h \
	../shared/include/linker-symbols.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds \
	../shared/startup/linkcmds.base \
	startup/linkcmds.t32mppc

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# Startup
libbsp_a_SOURCES += \
	../../shared/bootcard.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bsppredriverhook.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../../shared/bspclean.c \
	../../shared/bspgetworkarea.c \
	../shared/src/ppc-exc-handler-table.c \
	../shared/src/tictac.c \
	../shared/src/bsp-start-zero.S \
	startup/bspstart.c \
	startup/bspreset.c

# Clock
libbsp_a_SOURCES += clock/clock-config.c

# Timer
libbsp_a_SOURCES += ../../shared/timerstub.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console.c

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
	../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel

if HAS_SMP
libbsp_a_SOURCES += ../../shared/bspsmp.c
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
