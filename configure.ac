#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([libxint], [0.2], [])
AC_CONFIG_SRCDIR([include/libxint/uint.hpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

AC_LANG([C++])

#AX_CXX_COMPILE_STDCXX([20], [ext])
AX_APPEND_COMPILE_FLAGS([-std=c++20], [CXX])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 include/libxint/Makefile
                 examples/Makefile
                 tests/Makefile
                 tests/catch2/Makefile
                 tests/utils/Makefile])
AC_OUTPUT
