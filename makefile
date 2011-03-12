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
	$(COREDIR)ConfigReader.o $(COREDIR)Global.o $(COREDIR)Reply.o
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




Admin.so: $(admin_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(admin_objects) $(CXXFLAGS)
ChannelBot.so: $(channelbot_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(channelbot_objects) $(CXXFLAGS)
OCommands.so: $(ocommands_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(ocommands_objects) $(CXXFLAGS)
Support.so: $(support_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(support_objects) $(CXXFLAGS)
Znc.so: $(znc_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(znc_objects) $(CXXFLAGS)

main_objects = $(SRCDIR)main.o $(SRCDIR)Socket.o $(SRCDIR)IrcSocket.o \
	$(SRCDIR)Global.o $(SRCDIR)IrcData.o $(SRCDIR)Database.o $(SRCDIR)Bot.o $(SRCDIR)Parse.o \
	$(SRCDIR)Users.o $(SRCDIR)User.o $(SRCDIR)Channels.o $(SRCDIR)Channel.o $(SRCDIR)ChannelAuth.o \
	$(SRCDIR)ConfigReader.o $(SRCDIR)Reply.o $(SRCDIR)Data.o
admin_objects = $(MODULEDIR)Admin.o
support_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Support.o
channelbot_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)ChannelBot.o
ocommands_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)OCommands.o
authserv_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Authserv.o $(MODULEDIR)UserManagement.o
nickserv_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Nickserv.o $(MODULEDIR)UserManagement.o
znc_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Znc.o
