CC = g++
CFLAGS = -march=native -pipe #-O#n for nondebug
CXXFLAGS = $(CFLAGS) -fPIC -Wall -g #-g << debugging
BOOSTLIB = -lboost_thread #depends on arch	linux
#BOOSTLIB = -lboost_thread-mt #depends on arch	wine
LIBS = -ldl $(BOOSTLIB)
FLAGS = -I/usr/local/include
MAKEFLAGS = -j5 #-j#n for threaded compiling

TOPDIR=../../../
SRCDIR=src/
LIBDIR=$(TOPDIR).libs/
module_name=OCommands
objects=$(module_name).o
output=$(module_name).so

default: $(output)
Debug: all
Release: all
all: default


$(output): $(objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(objects) $(CXXFLAGS) $(FLAGS) $(LIBS)

cleanDebug: clean
cleanRelease: clean
clean:
	rm -f $(objects) $(LIBDIR)$(output)

cleanall: clean


