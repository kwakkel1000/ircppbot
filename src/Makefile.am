## Process this file with automake to produce Makefile.in

GIT_VERSION = $(shell sh -c 'git describe --abbrev=4 --dirty --always')

ACLOCAL_AMFLAGS = -I ../m4

bin_PROGRAMS = ircppbot

ircppbot_includedir = $(includedir)/include

ircppbot_include_HEADERS = \
	include/irc.h \
	include/ircdata.h \
	include/reply.h \
	include/bot.h \
	include/management/channels.h \
	include/management/channel.h \
	include/management/users.h \
	include/management/user.h \
	include/management/management.h \
	include/config.h

ircppbot_SOURCES = \
	irc.cpp \
	ircdata.cpp \
	reply.cpp \
	bot.cpp \
	management/channels.cpp \
	management/channel.cpp \
	management/users.cpp \
	management/user.cpp \
	management/management.cpp \
	main.cpp


ircppbot_CPPFLAGS = $(IRCPPBOT_CPPFLAGS)
ircppbot_LDFLAGS = $(IRCPPBOT_LDFLAGS)
ircppbot_LDADD = $(IRCPPBOT_LIBS)
