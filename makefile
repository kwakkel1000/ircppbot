CC = g++
CFLAGS = -march=native -pipe #-O#n for nondebug
CXXFLAGS = $(CFLAGS) -fPIC -Wall -g #-g << debugging
MYSQLFLAGS = -L/usr/include/mysql -lmysqlclient -I/usr/include/mysql #depends on arch 	linux
#MYSQLFLAGS = -L/usr/include/mysql -lmysqlclient -I/usr/include/mysql -L/usr/local/lib/mysql -lz #depends on arch 	wine
BOOSTLIB = -lboost_thread #depends on arch	linux
#BOOSTLIB = -lboost_thread-mt #depends on arch	wine
LIBS = -ldl $(BOOSTLIB)
MAKEFLAGS = -j5 #-j#n for threaded compiling

TOPDIR=
LIBDIR=$(TOPDIR).libs/
SRCDIR=src/
COREDIR=$(SRCDIR)core/
MANAGEMENTDIR=$(SRCDIR)management/
SOCKETDIR=$(SRCDIR)socket/
EXECUTABLE=bot

allfunctions = bot

core_objects = $(COREDIR)main.o $(COREDIR)Bot.o $(COREDIR)Parse.o \
	$(COREDIR)Data.o $(COREDIR)IrcData.o \
	$(COREDIR)Database.o $(COREDIR)Auth.o \
	$(COREDIR)ConfigReader.o $(COREDIR)Global.o $(COREDIR)Reply.o $(COREDIR)ModuleBase.o
management_objects = $(MANAGEMENTDIR)ChannelAuth.o \
	$(MANAGEMENTDIR)Channels.o $(MANAGEMENTDIR)Channel.o \
	$(MANAGEMENTDIR)Users.o $(MANAGEMENTDIR)User.o
socket_objects = $(SOCKETDIR)IrcSocket.o \
	$(SOCKETDIR)ServerSocket.o $(SOCKETDIR)Socket.o

Debug: all
Release: all
cleanDebug: clean
cleanRelease: clean

all: $(allfunctions)
cleanall: clean

default: bot

bot: $(core_objects) $(management_objects) $(socket_objects)
	$(CC) -rdynamic -o $(EXECUTABLE) $(core_objects) $(management_objects) $(socket_objects) $(MYSQLFLAGS) $(LIBS) $(CXXFLAGS)

clean:
	rm -f $(core_objects) $(management_objects) $(socket_objects) $(EXECUTABLE)
