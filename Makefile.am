lib_LTLIBRARIES = OneVsOne.la

OneVsOne_la_SOURCES = OneVsOne.cpp
OneVsOne_la_CXXFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
OneVsOne_la_LDFLAGS = -module -avoid-version -shared
OneVsOne_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	README.txt \
	OneVsOne.def \
	OneVsOne.sln \
	OneVsOne.vcxproj

MAINTAINERCLEANFILES =	\
	Makefile.in
