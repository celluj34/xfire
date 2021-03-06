#
# Most of this configure script was ripped from gaim or the
# gaim-encryption configure script, thanks guys ;)
#

AC_INIT(src/xfire.c)
AC_PREREQ([2.50])

XFIRE_VERSION="`cat ./VERSION`"
AM_INIT_AUTOMAKE([gaim-xfire], $XFIRE_VERSION, 'no-define')
AC_DEFINE_UNQUOTED(XFIRE_VERSION, "$XFIRE_VERSION", [Gaim-XFIRE Version])

AC_PATH_PROG(sedpath, sed)
#
# Our header
#

AH_TOP([ /* our header */
#ifndef _XFIRE_CONFIG_H
#define _XFIRE_CONFIG_H
])
AH_BOTTOM([
#endif /* _XFIRE_CONFIG_H */
])

#
# Look for the C compiler
#
CFLAGS_save="$CFLAGS"
AC_PROG_CC
CFLAGS="$CFLAGS_save"

AC_ARG_ENABLE(debug,   [  --enable-debug          compile with debugging support],,enable_debug=no)

if test "x$enable_debug" = "xyes" ; then
	DEBUG_CFLAGS="$DEBUG_CFLAGS -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED"
	AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
fi

AC_SUBST(DEBUG_CFLAGS)

if test "x$GCC" = "xyes"; then
	CFLAGS="$CFLAGS -Wall -g3"
fi
AC_SUBST(CFLAGS)

#
# Check for Gaim
#

PKG_CHECK_MODULES(GAIM, gaim,
[
   AC_DEFINE(HAVE_GAIM, 1, [Define if we've found gaim.])
])
AC_SUBST(GAIM_CFLAGS)
AC_SUBST(GAIM_LIBS)

#
# check for gtk and other dependencies
#

AM_PATH_GLIB_2_0(2.0.0,,AC_MSG_ERROR([
*** GLib 2.0 is required to build gaim-xfire; please make sure you have
*** the GLib development headers installed. The latest version of GLib is 
*** always available at http://www.gtk.org/.]))
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

#
# Check if plugins are enabled
#
want_plugins=yes
AM_CONDITIONAL(PLUGINS, test "$want_plugins" = "yes")

#
# Setup libtool
#
AM_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"

#
# Finish up
#
AM_CONFIG_HEADER(pre_config.h)
AC_OUTPUT([Makefile
		   gaim-xfire.spec
		   pixmaps/Makefile
		   src/Makefile
		  ])

echo;
echo Configuration complete
echo;
echo Debugging enabled..............: $enable_debug
echo;
echo Type make to compile
echo;
