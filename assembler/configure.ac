#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT(Piclang Assembler, 0.1, david@davecoss.com)
AC_CONFIG_SRCDIR([Parser.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_ARG_WITH([dnstd],AS_HELP_STRING([--with-dnstd],[Include DNStd library]),with_dnstd=${withval},AC_MSG_RESULT(Assuming libdnstd is installed in the directory above this one (or in system directories)))
if test x$with_dnstd != x; then
   AC_SUBST(LIBDNSTD_CPPFLAGS,"-I ${with_dnstd}/include/")
   AC_SUBST(LIBDNSTD_LDFLAGS,"-L ${with_dnstd}/lib")
   AC_SUBST(LIBDNSTD_LDADD,"-ldnstd")
fi

#For readline!
AC_ARG_WITH([readline],
            [AS_HELP_STRING([--without-readline],
              [disable support for readline])],
            [],
            [with_readline=yes])
          
          LIBREADLINE=
          AS_IF([test "x$with_readline" != xno],
            [AC_CHECK_LIB([readline], [main],
              [AC_SUBST([LIBREADLINE], ["-lreadline -lncurses"])
               AC_DEFINE([USE_READLINE], [1],
                         [Define if you have libreadline])
              ],
              [AC_MSG_FAILURE(
                 [readline test failed (--without-readline to disable)])],
              [-lncurses])])


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
