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
#include "y2savelogs.h"
#include <qpopupmenu.h>


Y2ControlCenter::Y2ControlCenter( WFlags wflags )
    : QMainWindow( 0, 0, wflags )	// parent, name, wflags
{
    setAppIcon();
    initStatusBar();

    _view = new Y2ControlCenterView( this );
    connect(_view, SIGNAL(statusMsg(const QString&)), this, SLOT(slotStatusHelpMsg(const QString&)));
    setCentralWidget( _view );

    initMenuBar();

    y2LogSaver = new Y2SaveLogs();
    connect(y2LogSaver, SIGNAL(statusMsg(const QString&)), this, SLOT(slotStatusHelpMsg(const QString&)));
}


Y2ControlCenter::~Y2ControlCenter()
{
   delete y2LogSaver; 
}


void Y2ControlCenter::setAppIcon()
{
    QString iconName( ICON_DIR "/yast.png" ); 
    QPixmap appIcon( iconName );

    if ( ! appIcon.isNull() )
    {
	// fprintf( stderr, "y2controlcenter: Using icon %s\n", iconName.ascii() );
	topLevelWidget()->setIcon( appIcon );
    }
    else
    {
	fprintf( stderr, "y2controlcenter: No icon %s\n", iconName.ascii() );
    }
}


void Y2ControlCenter::configure()
{
    statusBar()->message(_("Reading module list..."));

    if ( !_view->init() )
    {
	QString msg = *_view->getErrorString();
	statusBar()->message(_("*** Reading module list failed ***"));
	QMessageBox::information(this,_("YaST2 Control Center"),msg,_("Quit"));
	msg=(char*)0L;
	qApp->quit();
    }
}


void Y2ControlCenter::initMenuBar()
{
    //
    // "File" menu
    //
    
    QPopupMenu * fileMenu = new QPopupMenu( this );

    // "File" menu
    menuBar()->insertItem( _( "&File" ), fileMenu );

    // "File" -> "Quit" menu item - quits the application
    fileMenu->insertItem( _( "&Quit" ), qApp, SLOT( quit() ), CTRL + Key_Q );


    //
    // "Edit" menu
    //
    
    QPopupMenu * editMenu = new QPopupMenu( this );

    // "Edit" menu
    menuBar()->insertItem( _( "&Edit" ), editMenu );

    // "Edit" -> "Search" menu item - searches for YaST2 modules
    editMenu->insertItem( _( "&Search..." ), _view, SLOT( search() ), Key_F3 );


    
    //
    // "Help" menu
    //
    
    QPopupMenu * helpMenu = new QPopupMenu( this );

    // "Help" menu
    menuBar()->insertItem( _( "&Help" ), helpMenu );

    // "Help" -> "Help" menu item - searches for YaST2 modules
    helpMenu->insertItem( _( "&Help" ), _view, SLOT( help() ), Key_F1 );


    
}

void Y2ControlCenter::initStatusBar()
{
    statusBar ()->message ("", 2000);
}


void Y2ControlCenter::slotStatusHelpMsg(const QString &text)
{
    statusBar()->message(text, 4000);
}

bool Y2ControlCenter::eventFilter( QObject *obj, QEvent *ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
        QKeyEvent *k = (QKeyEvent*)ev;

        if ( k->key() == Qt::Key_F8    &&       // Shift-F8: save y2logs
	     k->state() == Qt::ShiftButton )
	{
	    y2LogSaver->save();
	    return true; // don't process this event any further
        }
    }
    
    return QMainWindow::eventFilter( obj, ev );
}

// override session management
void QApplication::saveState(QSessionManager & sm)
{
    sm.setRestartHint( QSessionManager::RestartNever );
}

// override session management
void QApplication::commitData(QSessionManager & sm)
{
}

#include <y2controlcenter.moc.cpp>
