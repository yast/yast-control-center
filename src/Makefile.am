#
# Makefile.am for modules/y2m_menu
#

AM_CXXFLAGS = -DICONDIR=\"$(imagedir)\" -DCONFIGDIR=\"$(configdir)\"	\
	-DLOCALEDIR=\"${localedir}\"

MOC = $(QTDIR)/bin/moc

client_DATA = pre-start.ycp

ydata_DATA = menu-translate.conf

ybin_PROGRAMS = y2controlcenter

y2controlcenter_SOURCES = \
	yastmodules.cpp		\
	y2controlcenterview.cpp	\
	y2controlcenter.cpp	\
	main.cpp		\
	searchdialog.cpp	\
	process.cpp	\
	y2cc_iconview.cpp	\
	y2cc_config.cpp	\
	myintl.h		\
	resource.h		\
	searchdialog.h		\
	y2controlcenter.h	\
	y2controlcenterview.h	\
	process.h	\
	yastmodules.h	\
	y2cc_config.h	\
	y2cc_globals.h	\
	y2cc_iconview.h \
	suseicon.xpm

y2controlcenter_METASOURCES = \
	searchdialog.moc.cpp \
	y2controlcenter.moc.cpp \
	y2controlcenterview.moc.cpp \
	process.moc.cpp \
	y2cc_iconview.moc.cpp \
	yastmodules.moc.cpp

y2controlcenter_LDADD = -L$(QTLIBDIR) -lqt-mt

searchdialog.o:			searchdialog.moc.cpp
y2controlcenter.o:		y2controlcenter.moc.cpp
y2controlcenterview.o:		y2controlcenterview.moc.cpp
yastmodules.o:			yastmodules.moc.cpp
process.o:			process.moc.cpp
y2cc_iconview.o:		y2cc_iconview.moc.cpp

searchdialog.moc.cpp:		searchdialog.h
y2controlcenter.moc.cpp:	y2controlcenter.h
y2controlcenterview.moc.cpp:	y2controlcenterview.h
yastmodules.moc.cpp:		yastmodules.h
process.moc.cpp:		process.h
y2cc_iconview.moc.cpp:		y2cc_iconview.h

%.moc.cpp: %.h
	$(MOC) $< -o $@

menu-translate.conf: $(y2controlcenter_SOURCES)
	./make_translate_conf *.cpp

# set the include path for X, qt and KDE
INCLUDES= -I$(QTDIR)/include

#y2controlcenter_METASOURCES = AUTO

# the library search path.
y2controlcenter_LDFLAGS = $(all_libraries)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(y2controlcenter_METASOURCES)
CLEANFILES = *.moc.cc

EXTRA_DIST = make_translate_conf $(client_DATA)
