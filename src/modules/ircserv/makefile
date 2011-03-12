CC = g++
CFLAGS = -march=native -pipe #-O#n for nondebug
CXXFLAGS = $(CFLAGS) -fPIC -Wall -g #-g << debugging
BOOSTLIB = -lboost_thread #depends on arch	linux
#BOOSTLIB = -lboost_thread-mt #depends on arch	wine
LIBS = -ldl $(BOOSTLIB) -lsnmp
FLAGS = -I/usr/local/include
MAKEFLAGS = -j5 #-j#n for threaded compiling

TOPDIR=../../../
SRCDIR=src/
MODULEDIR=../
LIBDIR=$(TOPDIR).libs/

authserv_objects = $(MODULEDIR)ModuleBase.o Authserv.o UserManagement.o
nickserv_objects = $(MODULEDIR)ModuleBase.o Nickserv.o UserManagement.o

default: Authserv.so Nickserv.so
Debug: all
Release: all
all: default


Authserv.so: $(authserv_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(authserv_objects) $(CXXFLAGS)
Nickserv.so: $(nickserv_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(nickserv_objects) $(CXXFLAGS)

cleanDebug: clean
cleanRelease: clean
clean:
	rm -f $(authserv_objects) $(nickserv_objects) $(LIBDIR)Authserv.so $(LIBDIR)Nickserv.so

cleanall: clean



