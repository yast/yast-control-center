 /****************************************************************************
|
| Copyright (c) 2011 Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
 \***************************************************************************/


#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QTimer>

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "main_window.h"
#include "i18n.h"

void printhelpandexit()
{
    cout << "Usage:"
	 << "\n   y2controlcenter [OPTIONS]"
	 << "\n"
	 << "\nOPTIONS:"
	 << "\n"
	 << "\n    --help         -h    this message"
	 << "\n    --fullscreen         use full screen"
	 << "\n    --noborder           no window manager border for main window"
	 << endl;
    exit (0);
}

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    set_textdomain("control-center");

    bool fullscreen = false;
    bool noborder = false;

    for ( int i=0 ;i < argc ;i++ )
    {
	QString opt = argv[i];

	// Normalize command line option - accept "--xy" as well as "-xy"

	if ( opt.startsWith( "--" ) )
	    opt.remove(0, 1);

	if ( opt == "-h" || opt == "-help" )	
	    printhelpandexit();
	else if ( opt == "-fullscreen" )	
	    fullscreen = true;
	else if ( opt == "-noborder" )	
	    noborder   = true;
    }
	   
    Qt::WindowFlags wflags = Qt::Window;

    if ( noborder )
	wflags |= Qt::FramelessWindowHint;

    MainWindow mainWin ( wflags );

    mainWin.setFullScreen ( fullscreen );
    mainWin.setNoBorder( noborder );
    if (QIcon::hasThemeIcon("yast"))
    {
	mainWin.setWindowIcon( QIcon::fromTheme( "yast" ) );
    }
    else
    {
	mainWin.setWindowIcon( QIcon( "/usr/share/pixmaps/yast.png"  ) );
    }

    if ( fullscreen )
    {
	QRect available = app.desktop()->availableGeometry();
	mainWin.resize( available.width(), available.height() );
	mainWin.move( available.topLeft() );
    }

    mainWin.show();

    QTimer::singleShot(60, &mainWin, SLOT( initialMsg() ));
    
    return app.exec();

}
