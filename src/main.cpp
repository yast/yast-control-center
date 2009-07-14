#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QTimer>

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "main_window.h"

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
    QApplication app(argc, argv);

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
	   
    Qt::WFlags wflags = Qt::Window;

    if ( noborder )
	wflags |= Qt::FramelessWindowHint;

    MainWindow mainWin ( wflags );

    mainWin.setFullScreen ( fullscreen );
    mainWin.setNoBorder( noborder );

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
