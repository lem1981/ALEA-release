#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(ALEA, 0.1, l.mukhanov@qub.ac.uk)
AC_CONFIG_SRCDIR([kernel/alea_threads.c])
AC_CONFIG_HEADERS([config.h])

AC_SUBST([prefix])
# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([clock_gettime gettimeofday pow strchr strrchr])

AC_CONFIG_FILES([Makefile
                 kernel/Makefile
                 kernel/power/ina231/Makefile
                 kernel/power/rapl/Makefile
                 kernel/sample/Makefile
                 kernel/sample/arm/Makefile
                 kernel/sample/x86/Makefile
                 scripts/Makefile])
AC_OUTPUT
