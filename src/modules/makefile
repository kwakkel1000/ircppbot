MAKEFLAGS	= -j5
DIRS	= ircserv ChannelBot OCommands Support Znc Snmp Example

all: $(DIRS) force_look
	-for d in $(DIRS); do (echo make $$d; cd $$d; $(MAKE)); done

clean:
	echo cleaning up in .
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done

force_look :
	true

