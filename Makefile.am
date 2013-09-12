## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

DIST_SUBDIRS = \
        src \
        man \
	src

SUBDIRS = \
        src\
        man \
	src

update:
	git pull

dist-hook:
	mkdir $(distdir)/conf
	cp -p conf/conf.ini.example $(distdir)/conf
	mkdir $(distdir)/mysql
	cp -p mysql/bot.sql mysql/binds.sql $(distdir)/mysql
