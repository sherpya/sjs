MAINTAINERCLEANFILES = Makefile.in
SUBDIRS=plugins

INCLUDES = -I@top_srcdir@ -I@smincdir@ -DSJS_LIBEXEC=\"$(sjs_libexec)\"

bin_PROGRAMS = sjs
sjs_SOURCES = sjs.cpp file.cpp versions.cpp plugin_mgr.cpp
sjs_LDADD = $(LIBS)
