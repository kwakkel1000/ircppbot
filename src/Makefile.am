## Process this file with automake to produce Makefile.in

GIT_VERSION = $(shell sh -c 'git describe --abbrev=4 --dirty --always')

ACLOCAL_AMFLAGS = -I ../m4

bin_PROGRAMS = ircppbot

ircppbot_includedir = $(includedir)/ircppbot

ircppbot_include_HEADERS = \
	include/moduleinterface.h \
	include/irc.h \
	include/ircdata.h \
	include/reply.h \
	include/binds.h \
	include/bot.h \
	include/management/managementscontainer.h \
	include/management/channel.h \
	include/management/user.h \
	include/management/auth.h \
	include/management/management.h \
	include/config.h

ircppbot_SOURCES = \
	irc.cpp \
	ircdata.cpp \
	reply.cpp \
	binds.cpp \
	bot.cpp \
	management/channel.cpp \
	management/user.cpp \
	management/auth.cpp \
	management/management.cpp \
	main.cpp


ircppbot_CPPFLAGS = $(IRCPPBOT_CPPFLAGS)
ircppbot_LDFLAGS = $(IRCPPBOT_LDFLAGS)
ircppbot_LDADD = $(IRCPPBOT_LIBS)
