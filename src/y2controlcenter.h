/***************************************************************************
                              y2controlcenter.h
                             -------------------
    begin               : Mit Okt 18 14:21:09 CEST 2000
    Copyright           : (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef Y2CONTROLCENTER_H
#define Y2CONTROLCENTER_H

// include files for QT
#include <qstring.h>
#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qaccel.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qapp.h>
#include <qstatusbar.h>
#include <qwhatsthis.h>
#include <qmsgbox.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qsessionmanager.h>
// #include <qprogressbar.h>

// application specific includes
#include "myintl.h"
#include "y2controlcenterview.h"
#include "y2savelogs.h"

/**
  * This Class is the base class for your application. It sets up the main
  * window and providing a menubar, toolbar
  * and statusbar. For the main view, an instance of class Y2ControlCenterView is
  * created which creates your view.
  */
class Y2ControlCenter : public QMainWindow
{
    Q_OBJECT

public:

    Y2ControlCenter( WFlags wflags = WType_TopLevel );
    ~Y2ControlCenter();


public slots:

    /** fill view with contents*/
    void configure();


    /** change the status message of the whole statusbar temporary */
    void slotStatusHelpMsg(const QString &text);


protected:

    void initMenuBar();
    void initStatusBar();
    bool eventFilter( QObject *obj, QEvent *ev );


private:

    Y2ControlCenterView * _view;
    Y2SaveLogs * y2LogSaver;
    void askSaveLogs();
    QString askForSaveFileName( const QString & startWith,
                                  const QString & filter,
                                  const QString & headline );

};
#endif

