/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    copyright            : (C) 2000 by SuSE GmbH
    author               : lnussel@suse.de
 ***************************************************************************/

/*
  Textdomain "control-center"
*/

#include <qapplication.h>
#include <qtimer.h>
#include <qfont.h>
#include <qstring.h>

#include <unistd.h>

// for MAXHOSTNAMELEN
#include <rpc/types.h>

// exit
#include <stdlib.h>

// strcmp
#include <string.h>

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "myintl.h"
#include "y2controlcenter.h"
#include "y2cc_config.h"
#include "suseicon.xpm"

//global class with config options
Config config;

void printhelpandexit()
{
    cout << "Usage:"
	 << "\n   y2controlcenter [OPTIONS]"
	 << "\n"
	 << "\nOPTIONS:"
	 << "\n"
	 << "\n    --help         -h    this message"
	 << "\n    --root         -r    also show \"root only\" modules"
	 << "\n    --quickstart   -Q	disable quick start"
	 << "\n    --fullscreen         use full screen"
	 << "\n    --noborder           no window manager border for main window"
	 << endl;
    exit (0);
}

int main( int argc, char *argv[] )
{
    //  cerr << time(0) << " main()" << endl;
    config.textdomain = "control-center";

    set_textdomain (config.textdomain);

    QApplication app( argc, argv );

    for ( int i=0 ;i < argc ;i++ )
    {
	QString opt = argv[i];

	// Normalize command line option - accept "--xy" as well as "-xy"

	if ( opt.startsWith( "--" ) )
	    opt.remove(0, 1);

	if 	( opt == "-h" || opt == "-help"		)	printhelpandexit();
	else if ( opt == "-r" || opt == "-root"		)	config.isroot     = true;
	else if ( opt == "-Q" || opt == "-quickstart"	)	config.QuickStart = false;
	else if ( opt == "-fullscreen"			)	config.fullscreen = true;
	else if ( opt == "-noborder"			)	config.noBorder   = true;
    }

    //  cout << (config.isroot.value()==true?"true":"false") << endl;

    Qt::WFlags wflags = Qt::WType_TopLevel;

    if ( config.noBorder )
	wflags |= Qt::WStyle_Customize | Qt::WStyle_NoBorder;

    Y2ControlCenter y2cc( wflags );

    if ( config.fullscreen )
    {
	QRect available = app.desktop()->availableGeometry();
	y2cc.resize( available.width(), available.height() );
	y2cc.move( available.topLeft() );
    }
    else
	y2cc.resize( 800, 580 );

    app.setMainWidget( &y2cc );


    QString title = _("YaST2 Control Center");
    char hostname[ MAXHOSTNAMELEN+1 ];
    if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
    {
	hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated
	
	if ( strlen( hostname ) > 0 && strcmp( hostname, "(none)" ) != 0 )
	{
	    title += " @ ";
	    title += hostname;
	}
    }
    y2cc.setCaption( title );
    y2cc.setIcon(QPixmap((const char **)suseicon_xpm));
    y2cc.show();

    QTimer::singleShot( 0, &y2cc, SLOT( configure() ) );

    return app.exec();
}

// vim: sw=4
