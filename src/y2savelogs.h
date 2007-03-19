/***************************************************************************
   y2savelogs.h  -  Save YaST Logs
   author              : tgoettlicher@suse.de
 ***************************************************************************/

#ifndef Y2SAVELOGS_H
#define Y2SAVELOGS_H

// include files for QT
#include <qstring.h>
#include <qprocess.h>


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

