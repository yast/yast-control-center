/***************************************************************************
    y2controlcenter.cpp  -  QMainWindow, originally created by kdevelop
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    copyright            : (C) 2000 by SuSE GmbH
    author               : lnussel@suse.de
 ***************************************************************************/

/*
  Textdomain "control-center"
*/

#include "y2controlcenter.h"
#include <unistd.h>

Y2ControlCenter::Y2ControlCenter()
{
  //setCaption("Y2ControlCenter " VERSION);

  progressbar=0L;

  initStatusBar();

  initView();
}

Y2ControlCenter::~Y2ControlCenter()
{
	delete view;
}

void Y2ControlCenter::initStatusBar()
{
  statusBar()->message(IDS_STATUS_DEFAULT, 2000);
}

void Y2ControlCenter::initView()
{
  // set the main widget here
  view=new Y2ControlCenterView(this);
  connect(view, SIGNAL(quit()), this, SLOT(slotFileQuit()));
  connect(view, SIGNAL(statusmsg(const QString&)), this, SLOT(slotStatusHelpMsg(const QString&)));
  connect(view, SIGNAL(sig_percentread(int)), this, SLOT(slotProgress(int)));
  setCentralWidget(view);
}

void Y2ControlCenter::configure()
{
  statusBar()->message(_("Reading module list..."));
//  progressbar=new QProgressBar(100,this);
//  statusBar()->addWidget(progressbar);

  if (!view->init())
  {
	  QString msg=*view->getErrorString();
	  statusBar()->message(_("*** Reading module list failed ***"));
  	  QMessageBox::information(this,_("YaST2 Control Center"),msg,_("Quit"));
	  msg=(char*)0L;
//	  delete view;
//	  qApp->quit();
	  slotFileQuit();
  }

 // statusBar()->removeWidget(progressbar);
//  delete progressbar;
//  progressbar=0L;
}

bool Y2ControlCenter::queryExit()
{
  int exit=QMessageBox::information(this, "Quit...",
                                    "Do your really want to quit?",
                                    QMessageBox::Ok, QMessageBox::Cancel);

  if (exit==1)
  {

  }
  else
  {

  };

  return (exit==1);
}

void Y2ControlCenter::slotFileQuit()
{
//  statusBar()->message("Exiting application...");
  // exits the Application

  qApp->quit();

//  statusBar()->message(IDS_STATUS_DEFAULT);
}

void Y2ControlCenter::slotStatusHelpMsg(const QString &text)
{
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 4000);
}

void Y2ControlCenter::slotProgress(int progress)
{
  //Displays how many percent of modules are loaded
  QString msg=_("Loading: %1%");
  msg=msg.arg(progress);
  statusBar()->message(msg,1000);
}

#include <y2controlcenter.moc.cpp>
