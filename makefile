include MAKECONFIG
include MODULES

MAKEFLAGS		= $(FLAGS)

TOPDIR			=
LIBDIR			= $(TOPDIR).libs/
SRCDIR			= src/
COREDIR			= $(SRCDIR)core/
MANAGEMENTDIR	= $(SRCDIR)management/
SOCKETDIR		= $(SRCDIR)socket/


MODULE_DIR		= $(SRCDIR)modules/
EXECUTABLE		= bot

allfunctions 	= bot modules

core_objects 		= \
	$(COREDIR)main.o $(COREDIR)Bot.o $(COREDIR)Output.o \
	$(COREDIR)Data.o $(COREDIR)IrcData.o \
	$(COREDIR)Database.o $(COREDIR)DatabaseData.o \
	$(COREDIR)ConfigReader.o $(COREDIR)Global.o $(COREDIR)Reply.o $(COREDIR)ModuleBase.o $(COREDIR)BotLib.o
management_objects 	= \
	$(MANAGEMENTDIR)Whois.o $(MANAGEMENTDIR)WhoisDataContainer.o \
	$(MANAGEMENTDIR)ChannelAuth.o \
	$(MANAGEMENTDIR)Channels.o $(MANAGEMENTDIR)Channel.o \
	$(MANAGEMENTDIR)Users.o $(MANAGEMENTDIR)User.o
socket_objects 		= \
	$(SOCKETDIR)IrcSocket.o \
	$(SOCKETDIR)ServerSocket.o $(SOCKETDIR)Socket.o

default: bot

Debug: all
Release: all
cleanDebug: clean
cleanRelease: clean

all: $(allfunctions)
updateall: update updatemodules
cleanall: clean cleanmodules


bot: $(core_objects) $(management_objects) $(socket_objects)
	$(CC) -rdynamic -o $(EXECUTABLE) $(core_objects) $(management_objects) $(socket_objects) $(MYSQLFLAGS) $(LIBS) $(CXXFLAGS)

update:
	git pull

clean:
	rm -f $(core_objects) $(EXECUTABLE)
	rm -f $(management_objects)
	rm -f $(socket_objects)


modules: $(MODULES) force_look
	-for d in $(MODULES); do (echo make $$d; cd $$d; $(MAKE)); done

updatemodules: $(MODULES) force_look
	-for d in $(MODULES); do (echo make $$d; cd $$d; $(MAKE) update); done

cleanmodules:
	echo cleaning up in .
	-for d in $(MODULES); do (cd $$d; $(MAKE) clean ); done

force_look :
	true

