#
# Makefile.am for control-center/src
#

AM_CXXFLAGS = \
	-DICON_DIR=\"${themedir}/current/icons/32x32/apps\" 	\
	-DFALLBACK_ICON_DIR=\"${prefix}/share/icons/hicolor/32x32/apps\"	\
	-DPIXMAP_DIR=\"${themedir}/current/control-center\"	\
	-DMODULES_DESKTOP_DIR=\"${desktopdir}\"			\
	-DGROUPS_DESKTOP_DIR=\"${desktopdir}/groups\"		\
	-DLOCALEDIR=\"${localedir}\"				\
	-DDESKTOP_TRANSLATIONS_DIR=\"${datadir}/locale\"


MOC = $(QTDIR)/bin/moc

self_desktopdir=${datadir}/applications
self_desktop_DATA = YaST.desktop

self_icondir=${datadir}/pixmaps
self_icon_DATA=yast.png

ybin_PROGRAMS = y2controlcenter

y2controlcenter_SOURCES =	\
	QY2Settings.cpp		\
	myintl.cpp		\
	process.cpp		\
	searchdialog.cpp	\
	y2cc_config.cpp		\
	y2cc_iconview.cpp	\
	y2controlcenter.cpp	\
	y2controlcenterview.cpp	\
	yastmodules.cpp		\
	y2savelogs.cpp		\
	main.cpp

noinst_HEADERS =		\
	QY2Settings.h		\
	myintl.h 		\
	process.h		\
	searchdialog.h		\
	y2cc_config.h		\
	y2cc_globals.h		\
	y2cc_iconview.h		\
	y2controlcenter.h	\
	y2controlcenterview.h	\
	yastmodules.h		\
	y2savelogs.h

y2controlcenter_METASOURCES = 		\
	process.moc.cpp 		\
	searchdialog.moc.cpp 		\
	y2cc_iconview.moc.cpp 		\
	y2controlcenter.moc.cpp 	\
	y2controlcenterview.moc.cpp 	\
	yastmodules.moc.cpp

y2controlcenter_LDADD = -L$(QTLIBDIR) -lqt-mt

searchdialog.o:			searchdialog.moc.cpp
y2controlcenter.o:		y2controlcenter.moc.cpp
y2controlcenterview.o:		y2controlcenterview.moc.cpp
yastmodules.o:			yastmodules.moc.cpp
y2savelogs.o:			y2savelogs.moc.cpp
process.o:			process.moc.cpp
y2cc_iconview.o:		y2cc_iconview.moc.cpp

searchdialog.moc.cpp:		searchdialog.h
y2controlcenter.moc.cpp:	y2controlcenter.h
y2controlcenterview.moc.cpp:	y2controlcenterview.h
yastmodules.moc.cpp:		yastmodules.h
y2savelogs.moc.cpp:		y2savelogs.h
process.moc.cpp:		process.h
y2cc_iconview.moc.cpp:		y2cc_iconview.h

%.moc.cpp: %.h
	$(MOC) $< -o $@

# set the include path for X, qt and KDE
INCLUDES= -I$(QTDIR)/include

# the library search path.
y2controlcenter_LDFLAGS = $(all_libraries)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(y2controlcenter_METASOURCES)
CLEANFILES = *.moc.cpp

EXTRA_DIST = $(self_desktop_DATA) $(self_icon_DATA)
