#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mem], [0.1.0], [wuyonggao@gmail.com])
AC_CONFIG_SRCDIR([mem.c])
AC_CONFIG_HEADERS([config.h])

AC_ARG_VAR(READ_COUNT, number of pages read per time)

if test "${ac_env_READ_COUNT_set}" != "set"; then
  READ_COUNT=100
fi

AM_INIT_AUTOMAKE()
# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([strcspn strtoull])
AC_CONFIG_FILES(Makefile)

AC_OUTPUT
