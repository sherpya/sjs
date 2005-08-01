MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = *.so

INCLUDES = -I@top_srcdir@ -I@smincdir@
INCLUDES += -I@top_srcdir@/plugins/minizip
INCLUDES += -I@top_srcdir@/plugins/korax/util

lib_LTLIBRARIES = libcurl.la libzlib.la libkorax.la libregistry.la

plugindir = $(sjs_libexec)/plugins
plugin_DATA = curl.so zlib.so korax.so

libcurl_la_SOURCES = curl.cpp
libcurl_la_LDFLAGS = @PLUGIN_LDFLAGS@ -o curl.so
libcurl_la_LIBADD  = $(CURL) $(LIBS)

libzlib_la_SOURCES = minizip/unzip.c minizip/ioapi.c \
                     zlib.cpp zlib_classes.cpp
libzlib_la_LDFLAGS = @PLUGIN_LDFLAGS@ -o zlib.so
libzlib_la_LIBADD  = $(ZLIB) $(LIBS)

libkorax_la_SOURCES = korax.cpp \
                      korax/util/util_misc.cpp \
                      korax/util/util_par.cpp \
                      korax/util/util_sync.cpp \
                      korax/util/util_sys.cpp
libkorax_la_LDFLAGS = @PLUGIN_LDFLAGS@ -o korax.so
libkorax_la_LIBADD  = $(LIBS)

libregistry_la_SOURCES = registry.cpp win32_stubs.cpp
libregistry_la_LDFLAGS = @PLUGIN_LDFLAGS@ -o registry.so
libregistry_la_LIBADD  = $(LIBS)
