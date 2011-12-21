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

