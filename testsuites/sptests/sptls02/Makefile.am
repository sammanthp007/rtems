rtems_tests_PROGRAMS = sptls02
sptls02_SOURCES = init.cc

dist_rtems_tests_DATA = sptls02.scn sptls02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CXXFLAGS += -std=gnu++11 -ftls-model=local-exec

LINK_OBJS = $(sptls02_OBJECTS)
LINK_LIBS = $(sptls02_LDLIBS)

sptls02$(EXEEXT): $(sptls02_OBJECTS) $(sptls02_DEPENDENCIES)
	@rm -f sptls02$(EXEEXT)
	$(make-cxx-exe)

include $(top_srcdir)/../automake/local.am
