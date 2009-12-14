/***************************************************************************
   yqsavelogs.h  -  Save YaST Logs
   author              : tgoettlicher@suse.de
 ***************************************************************************/

#ifndef YQSAVELOGS_H
#define YQSAVELOGS_H

// include files for QT
#include <QString>
#include <QProcess>


// application specific includes
#include "i18n.h"

/**
  * This Class asks for a filename and saves YaST logfies 
  */
class YQSaveLogs  : public QObject
{
    Q_OBJECT

public:

    YQSaveLogs();
    ~YQSaveLogs();

public slots:
    void saveProcessExited( int exitCode, QProcess::ExitStatus exitStatus ); 
    void save();


protected:

signals:
    void statusMsg( const QString& msg );

private:

    bool lock;
    QProcess * proc;

    void SaveLogs();
    QString askForSaveFileName( const QString & startWith,
				const QString & filter,
				const QString & headline );

};
#endif

