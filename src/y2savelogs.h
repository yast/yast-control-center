/***************************************************************************
   y2savelogs.h  -  Save YaST Logs
   author              : tgoettlicher@suse.de
 ***************************************************************************/

#ifndef Y2SAVELOGS_H
#define Y2SAVELOGS_H

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
#include <qprocess.h>
#include <qsignal.h>
#include <qobject.h>

// application specific includes
#include "myintl.h"

/**
  * This Class asks for a filename and saves YaST logfies 
  */
class Y2SaveLogs  : QObject
{
    Q_OBJECT

public:

    Y2SaveLogs();
    ~Y2SaveLogs();

    void save();

public slots:
    void saveProcessExited(); 


protected:

private:

    bool lock;
    QProcess * proc;

    void SaveLogs();
    QString askForSaveFileName( const QString & startWith,
                                  const QString & filter,
                                  const QString & headline );

};
#endif

