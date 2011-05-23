include ../MODULE_CONFIG

CXX_CFLAGS 		=
CXX_LDFLAGS 	=
LIBRARIES 		= $(CXX_SYSLIBS)

MODULE_NAME		= UserManagement
OBJECTS			= $(MODULE_NAME).o
OUTPUT			= $(MODULE_NAME).so

default: $(OUTPUT)
Debug: all
Release: all
all: default


$(OUTPUT): $(SRC_DIR)$(MODULE_NAME).o
	$(CXX_LINK) -o $(LIB_DIR)$@ $(OBJECTS) $(LIBRARIES)

cleanDebug: clean
cleanRelease: clean
clean:
	rm -f $(OBJECTS) $(LIB_DIR)$(OUTPUT)

cleanall: clean

update:
