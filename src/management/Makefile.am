## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I ../../m4

noinst_LIBRARIES = libmanagement.a

libmanagement_a_includedir = $(includedir)/include/management

libmanagement_a_include_HEADERS = \
        ../include/management/channels.h \
        ../include/management/channel.h \
        ../include/management/users.h \
        ../include/management/user.h

libmanagement_a_SOURCES = \
        channels.cpp \
        channel.cpp \
        users.cpp \
        user.cpp

libmanagement_a_CPPFLAGS = $(IRCPPBOT_CPPFLAGS)
libmanagement_a_LDFLAGS = $(IRCPPBOT_LDFLAGS)
LIBS += $(IRCPPBOT_LIBS)
