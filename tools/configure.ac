#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.52])
AC_INIT(Piclang Assembler, 0.3, david@davecoss.com)
AC_CONFIG_SRCDIR([pasm.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# Checks for libraries.
AC_ARG_WITH([picos],
	AS_HELP_STRING([--with-picos],[Include PIC OS headers]),
	with_picos="$withval",
	[with_picos=no])

if test x"$with_picos" != x"no"; then
   AC_SUBST(PICOS_CFLAGS,"-I ${with_picos}")
   AC_MSG_RESULT(Using provided picos path $with_picos)
else
   AC_MSG_RESULT(Assuming picos headers are installed in the directory above this one or in system directories)
fi


#For readline!
#AC_ARG_WITH([readline],
#            [AS_HELP_STRING([--without-readline],
#              [disable support for readline])],
#            [],
#            [with_readline=yes])
#          
#          LIBREADLINE=
#          AS_IF([test "x$with_readline" != xno],
#            [AC_CHECK_LIB([readline], [main],
#              [AC_SUBST([LIBREADLINE], ["-lreadline -lncurses"])
#               AC_DEFINE([USE_READLINE], [1],
#                         [Define if you have libreadline])
#              ],
#              [AC_MSG_FAILURE(
#                 [readline test failed (--without-readline to disable)])],
#              [-lncurses])])


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
