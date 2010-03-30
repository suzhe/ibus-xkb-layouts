AC_INIT([ibus-xkb-layouts],
        [0.0.0],
        [http://github.com/suzhe/ibus-xkb-layouts])

AC_PREREQ([2.50])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.10])
AC_GNU_SOURCE

AM_DISABLE_STATIC
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_HEADER_STDC

PKG_PROG_PKG_CONFIG([0.20])

AC_ARG_WITH([xkb-base],
            AC_HELP_STRING([--with-xkb-base=${datadir}/X11/xkb],
                           [XKB base path]),
            [XKB_BASE=$withval],
            [XKB_BASE="/usr/share/X11/xkb"])

AC_SUBST([XKB_BASE])

# check ibus
PKG_CHECK_MODULES(IBUS, [ibus-1.0 >= 1.3.0])

# check libxklavier
PKG_CHECK_MODULES(LIBXKLAVIER, [libxklavier >= 4.0])

# OUTPUT files
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/xkb-layouts.xml.in
                 ])

AC_OUTPUT
