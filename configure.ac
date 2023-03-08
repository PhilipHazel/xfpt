dnl Process this file with autoconf to produce a configure script.

dnl This is required at the start; the name is the name of a file
dnl it should be seeing, to verify it is in the same directory.

AC_INIT
AC_CONFIG_SRCDIR([NOTICE])

dnl Checks for programs.

AC_PROG_CC
AC_PROG_INSTALL

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_CONST
AC_TYPE_SIZE_T

AC_SUBST(LDFLAGS)
AC_CONFIG_FILES(Makefile)

dnl This must be last; it determines what files are written
AC_OUTPUT

cat <<EOF

xfpt configuration summary:

    C compiler .......................: ${CC}
    C compiler flags .................: ${CFLAGS}
    Link flags .......................: ${LDFLAGS} 
    Install xfpt command in ..........: ${prefix}/bin
    Install shared data in ...........: ${prefix}/share/xfpt
    Install man page in ..............: ${prefix}/man 

EOF

dnl End of configure.ac
