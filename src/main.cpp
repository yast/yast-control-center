/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    copyright            : (C) 2000 by SuSE GmbH
    author               : lnussel@suse.de
 ***************************************************************************/

#include <qapplication.h>
#include <qtimer.h>
#include <qfont.h>
#include <qstring.h>

// for getuid
#include <unistd.h>
#include <sys/types.h>

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
    cout << "Usage:" << endl
	 << "	y2controlcenter [OPTIONS]" << endl << endl
	 << "OPTIONS:" << endl
	 << "	-r	Show root only modules too" << endl
	 << "	-Q	Disable quick start" << endl
	 << "	--help	This screen" << endl << endl;
    exit (0);
}

int main(int argc, char *argv[])
{
//  cerr << time(0) << " main()" << endl;
  config.isroot = getuid() == 0;
  config.QuickStart = true;
  config.textdomain = "menu";

  bindtextdomain (config.textdomain, LOCALEDIR);
  textdomain (config.textdomain);
  bind_textdomain_codeset (config.textdomain, "UTF-8");

  QApplication a(argc, argv);

  for (int i=0;i<argc;i++)
  {
    if(!strcmp(argv[i],"-r"))
    {
      config.isroot = true;
    }
    else if(!strcmp(argv[i],"--help"))
    {
      printhelpandexit();
    }
    else if(!strcmp(argv[i],"-Q"))
    {
      config.QuickStart = false;
    }
  }

//  cout << (config.isroot.value()==true?"true":"false") << endl;

  Y2ControlCenter *y2controlcenter=new Y2ControlCenter();
  y2controlcenter->resize(800,580);
  a.setMainWidget(y2controlcenter);



  QString title = _("YaST2 Control Center");
  char hostname[ MAXHOSTNAMELEN+1 ];
  if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
  {
      hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated
      title += " @ ";
      title += hostname;
  }
  y2controlcenter->setCaption( title );


  y2controlcenter->setIcon(QPixmap((const char **)suseicon_xpm));

  y2controlcenter->show();

  QTimer::singleShot(0,y2controlcenter, SLOT(configure()));

  int ret=a.exec();

  delete y2controlcenter;

  return ret;
}

// vim: sw=2
