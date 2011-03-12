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
LIBDIR=$(TOPDIR).libs/

objects = ../ModuleBase.o Test.o

default: Test.so
Debug: all
Release: all
all: default


Test.so: $(objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(objects) $(CXXFLAGS) $(FLAGS) $(LIBS)

cleanDebug: clean
cleanRelease: clean
clean:
	rm -f $(objects) $(LIBDIR)Test.so

cleanall: clean


