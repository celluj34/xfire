EXTRA_DIST = \
	x_create.h \
	x_packets.h \
	x_search.h \
	x_games.h \
	x_utils.h

plugindir = $(libdir)/gaim

libxfire_la_LDFLAGS = -module -avoid-version

if PLUGINS

plugin_LTLIBRARIES = libxfire.la

libxfire_la_SOURCES = \
	xfire.c \
	x_utils.c \
	x_create.c \
	x_search.c \
	x_games.c \
	x_packets.c 

libxfire_la_LIBADD = \
	$(GLIB_LIBS)

endif

AM_CPPFLAGS = \
	-DLIBDIR=\"$(libdir)/gaim/\" \
	-DDATADIR=\"$(datadir)\" \
	$(GLIB_CFLAGS) \
	$(GAIM_CFLAGS) \
	$(DEBUG_CFLAGS)
