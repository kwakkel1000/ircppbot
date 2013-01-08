#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([IRCPPBOT], [2.0], [VOID])
AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src/include/bot.h])
AC_CONFIG_HEADERS([src/include/config.h])
AC_CONFIG_MACRO_DIR([m4])
LT_PREREQ([2.2])
LT_INIT([dlopen])


# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_LANG([C++])
CXXFLAGS="$CXXFLAGS -Wall"
AX_CXX_COMPILE_STDCXX_11([noext])

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([gframe], [libgframe_is_present])
AC_CHECK_LIB([gframe_socket], [libgframe_socket_is_present])
AC_CHECK_LIB([gframe_database], [libgframe_database_is_present])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h syslog.h unistd.h], [], AC_MSG_ERROR([missing required header(s)]))

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([strerror])
AC_FUNC_FORK

LDFLAGS="$LDFLAGS -rdynamic"

IRCPPBOT_CPPFLAGS=" -D__GIT_VERSION=\\\"\${GIT_VERSION}\\\""
IRCPPBOT_LDFLAGS=""
IRCPPBOT_LIBS=""
AC_SUBST(IRCPPBOT_CPPFLAGS)
AC_SUBST(IRCPPBOT_LDFLAGS)
AC_SUBST(IRCPPBOT_LIBS)

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile])


AC_OUTPUT

echo "
  ($PACKAGE_NAME) version $PACKAGE_VERSION
  Prefix..........:    $prefix
  C++ Compiler....:    $CXX $CXXFLAGS $CPPFLAGS
  Linker..........:    $LD $LDFLAGS $LIBS

Configuration:

  Detected OS.....:    ${host_os}
  Install path....:    ${prefix}
  Compilation arch:    ${ARCH}

  Compiler........:    ${CC}
  Compiler flags..:    ${CFLAGS}


  IRCPPBOT details:
  Linker flags....:    $LDFLAGS $IRCPPBOT_LDFLAGS
  C++ Compiler....:    $CXX $CXXFLAGS $CPPFLAGS $IRCPPBOT_CPPFLAGS
  Libraries.......:    $LIBS $IRCPPBOT_LIBS
  gdb Debug.......:    ${gdbdebug}"

echo
echo "***********************************************************"
echo "*            Now run '${am_make}'                               *"
echo "*            And run '${am_make}' install                       *"
echo "*                                                         *"
echo "*           Thank you for using IRCPPBOT!                 *"
echo "*             <http://www.ircbot.info>                    *"
echo "***********************************************************"
echo

