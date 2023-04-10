lib_LTLIBRARIES = OneVsOne.la

OneVsOne_la_SOURCES = \
	UrlHandler.h \
	UrlHandler.cpp \
	OneVsOne.cpp
OneVsOne_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
OneVsOne_la_LDFLAGS = -module -avoid-version -shared
OneVsOne_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	README.md \
	LICENSE.md

MAINTAINERCLEANFILES =  \
	Makefile.in
