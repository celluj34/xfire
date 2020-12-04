EXTRA_DIST = \
	COPYING \
	ChangeLog \
	INSTALL \
	NEWS \
	README \
	VERSION \
	mkinstalldirs \
	install-sh \
	config.guess \
	config.sub \
	gaim-xfire.spec \
	Makefile.mingw \
	xfire-installer.nsi \
	xfire_config.h.mingw \
	nsis/header.bmp \
	nsis/install.ico \
	nsis/Thumbs.db \
	nsis/translations/english.nsh

SUBDIRS = src pixmaps

BUILT_SOURCES = xfire_config.h

CFLAGS=pkg-config --cflags --libs gaim

$(OBJECTS): $(BUILT_SOURCES)

xfire_config.h: pre_config.h
	$(sedpath) 's/#define PACKAGE/#define XFIRE_PACKAGE/g' pre_config.h > $@
