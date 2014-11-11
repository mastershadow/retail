#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(retail, 1.0, eduard@roccatello.it)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/retail.c])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(ncurses, initscr)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h pthread.h ncurses.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AM_INIT_AUTOMAKE([foreign])

AC_OUTPUT(Makefile src/Makefile)
