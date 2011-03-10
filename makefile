CC = g++
CFLAGS = -march=native -pipe #-O#n for nondebug
CXXFLAGS = $(CFLAGS) -fPIC -Wall -g #-g << debugging
MYSQLFLAGS = -L/usr/include/mysql -lmysqlclient -I/usr/include/mysql #depends on arch 	linux
#MYSQLFLAGS = -L/usr/include/mysql -lmysqlclient -I/usr/include/mysql -L/usr/local/lib/mysql -lz #depends on arch 	wine
BOOSTLIB = -lboost_thread #depends on arch	linux
#BOOSTLIB = -lboost_thread-mt #depends on arch	wine
LIBS = -ldl $(BOOSTLIB)
MAKEFLAGS = -j5 #-j#n for threaded compiling

SRCDIR=src/
MODULEDIR=$(SRCDIR)modules/
LIBDIR=.libs/
EXECUTABLE=bot

#allfunctions = src/bot Admin.so Authserv.so Nickserv.so ChannelBot.so OCommands.so Support.so Test.so
allfunctions = bot ChannelBot.so Authserv.so Nickserv.so OCommands.so Test.so

main_objects = $(SRCDIR)main.o $(SRCDIR)ServerSocket.o $(SRCDIR)Socket.o $(SRCDIR)IrcSocket.o \
	$(SRCDIR)Global.o $(SRCDIR)IrcData.o $(SRCDIR)Database.o $(SRCDIR)Bot.o $(SRCDIR)Parse.o \
	$(SRCDIR)Users.o $(SRCDIR)User.o $(SRCDIR)Channels.o $(SRCDIR)Channel.o $(SRCDIR)ChannelAuth.o \
	$(SRCDIR)ConfigReader.o $(SRCDIR)ReadWriteMutex.o $(SRCDIR)Reply.o $(SRCDIR)Data.o
admin_objects = $(MODULEDIR)Admin.o
support_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Support.o
channelbot_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)ChannelBot.o
ocommands_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)OCommands.o
authserv_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Authserv.o $(MODULEDIR)UserManagement.o
nickserv_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Nickserv.o $(MODULEDIR)UserManagement.o
test_objects = $(MODULEDIR)ModuleBase.o $(MODULEDIR)Test.o

default: bot

bot: $(main_objects)
	$(CC) -rdynamic -o $(EXECUTABLE) $(main_objects) $(MYSQLFLAGS) $(LIBS) $(CXXFLAGS)
Admin.so: $(admin_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(admin_objects) $(CXXFLAGS)
ChannelBot.so: $(channelbot_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(channelbot_objects) $(CXXFLAGS)
OCommands.so: $(ocommands_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(ocommands_objects) $(CXXFLAGS)
Authserv.so: $(authserv_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(authserv_objects) $(CXXFLAGS)
Nickserv.so: $(nickserv_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(nickserv_objects) $(CXXFLAGS)
Support.so: $(support_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(support_objects) $(CXXFLAGS)
Test.so: $(test_objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(test_objects) $(CXXFLAGS)

Debug: all
Release: all
cleanDebug: clean
cleanRelease: clean
clean:
	rm -f $(LIBDIR)*.so $(SRCDIR)*.o $(MODULEDIR)*.o $(EXECUTABLE)

all: $(allfunctions)
cleanall: clean

Admin: Admins.so
cleanAdmin:
	rm -f $(LIBDIR)Admin.so $(admin_objects)
rAdmin: cleanAdmin Admin.so

cleanChannelBot: ChannelBot.so
	rm -f $(LIBDIR)ChannelBot.so $(channelbot_objects)
rChannelBot: cleanChannelBot ChannelBot.so

OCommands: OCommands.so
cleanOCommands:
	rm -f $(LIBDIR)OCommands.so $(ocommands_objects)
rOCommands: cleanOCommands OCommands.so

Authserv: Authserv.so
cleanAuthserv:
	rm -f $(LIBDIR)Authserv.so $(authserv_objects)
rAuthserv: cleanAuthserv Authserv.so

Nickserv: Nickserv.so
cleanNickserv:
	rm -f $(LIBDIR)Nickserv.so $(nickserv_objects)
rNickserv: cleanNickserv Nickserv.so

Support: Support.so
cleanSupport:
	rm -f $(LIBDIR)Support.so $(support_objects)
rSupport: cleanSupport Support.so

Test: Test.so
cleanTest:
	rm -f $(LIBDIR)Test.so $(test_objects)
rTest: cleanTest Test.so
