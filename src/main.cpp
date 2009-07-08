#include <QApplication>
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
	 << "\n    --root         -r    also show \"root only\" modules"
	 << "\n    --fullscreen         use full screen"
	 << "\n    --noborder           no window manager border for main window"
	 << endl;
    exit (0);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    for ( int i=0 ;i < argc ;i++ )
    {
	QString opt = argv[i];

	// Normalize command line option - accept "--xy" as well as "-xy"

	if ( opt.startsWith( "--" ) )
	    opt.remove(0, 1);

	if ( opt == "-h" || opt == "-help" )	printhelpandexit();
    }
	   
    MainWindow mainWin;
    
    mainWin.show();
    
    QTimer::singleShot(60, &mainWin, SLOT( initialMsg() ));
    
    return app.exec();

}
