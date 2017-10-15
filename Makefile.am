#AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = myUI

#INCLUDES = liblinklist

#noist_PROGRAMS = myUI

myUI_SOURCES = callbacks.c \
	callbacks.h \
	interface.c \
	interface.h \
	support.c \
	support.h \
	main.c

#myUI_LDFLAGS = -export-dynamic

#myUI_SUBDIRS = liblinklist

myUI_CFLAGS = $(glib_CFLAGS) \
	$(gtk_CFLAGS) \
	-I${top_srcdir}/liblinklist/

myUI_CXXFLAGS = $(glib_CFLAGS) \
	$(gtk_CFLAGS) \
	-I${top_srcdir}/liblinklist/

myUI_LDADD = $(glib_LIBS) $(gtk_LIBS) ../liblinklist/liblinklist.a