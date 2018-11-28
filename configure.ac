AC_PREREQ(2.60)
# the "2" in AC_INIT is the version, and must match NEWS.
AC_INIT([amtrelayd],
        [2],
        [jakeholland.net@gmail.com],
        [amtrelayd],
        [https://github.com/GrumpyOldTroll/amtrelayd])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([
	check-news
	foreign
	1.11
	-Wall
	-Werror
	-Wno-portability
	tar-pax
	no-dist-gzip
	dist-xz
	subdir-objects
])
AC_PROG_CC_STDC
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([no])
LT_INIT()
AC_PREFIX_DEFAULT([/usr])

AC_PROG_SED
AC_PROG_MKDIR_P

AC_ARG_ENABLE([logging],
        AS_HELP_STRING([--disable-logging], [disable system logging @<:@default=enabled@:>@]),
        [], enable_logging=yes)
AS_IF([test "x$enable_logging" = "xyes"], [
        AC_DEFINE(ENABLE_LOGGING, [1], [System logging.])
])

AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug], [enable debug messages @<:@default=disabled@:>@]),
        [], [enable_debug=no])
AS_IF([test "x$enable_debug" = "xyes"], [
        AC_DEFINE(ENABLE_DEBUG, [1], [Debug messages.])
])

AC_CHECK_FUNCS([ \
	__secure_getenv \
	secure_getenv\
])

AC_CHECK_HEADER([event.h],
     [AC_DEFINE([HAVE_EVENT_H], [1], [Define to 1 if you have <event.h>.])],
     [AC_MSG_ERROR([could not include <event.h>. You need to install libevent-dev or build from https://github.com/libevent/libevent.git and set C_INCLUDE_PATH and LIBRARY_PATH])])

AC_CHECK_LIB([event],[event_base_new],
	[AC_DEFINE([HAVE_EVENT_LIB], [1], [Define to 1 if you have libevent])],
	[AC_MSG_ERROR([could not invoke event_base_new with -levent, you need to install libevent-dev or build from https://github.com/libevent/libevent.git and set C_INCLUDE_PATH and LIBRARY_PATH])])

my_CFLAGS="\
-Werror \
-Wall \
-Wchar-subscripts \
-Wformat-security \
-Wnested-externs \
-Wpointer-arith \
-Wshadow \
-Wtype-limits \
"
AC_SUBST([my_CFLAGS])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
        Makefile
])

AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE $VERSION
        =====

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}

        logging:                ${enable_logging}
        debug:                  ${enable_debug}
])
