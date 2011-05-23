/***************************************************************************
   yqsavelogs.cpp  -  Save YaST Logs 
   author               : tgoettlicher@suse.de
 ***************************************************************************/

/*
  Textdomain "control-center"
*/

#include "yqsavelogs.h"

#include <QFileDialog>
#include <QMessageBox>


YQSaveLogs::YQSaveLogs()
{
    lock = false;
    proc = 0;
}

YQSaveLogs::~YQSaveLogs()
{
}

void YQSaveLogs::save()
{

   if (lock)
   {
       QMessageBox::warning( 0,                                       // parent
                            "Warning",                                // caption
                            QString("Logs are written at the moment. Please try again later."),
                            QMessageBox::Ok | QMessageBox::Default,   // button0
                            QMessageBox::NoButton,                    // button1
                            QMessageBox::NoButton );                  // button2
       return;
   }
 
   SaveLogs();

}


void YQSaveLogs::saveProcessExited( int exitCode, QProcess::ExitStatus exitStatus )
{
    if ( exitStatus == QProcess::NormalExit )
    {
        emit statusMsg( _("Log files written successfully.") );
    }
    else
    {
        emit statusMsg( _("Couldn't save y2logs.") );
        QMessageBox::warning( 0,                                        // parent
                              "Error",                                  // caption
                               QString( "Couldn't save y2logs - "
                                     "exit code %1").arg( exitStatus ),
                               QMessageBox::Ok | QMessageBox::Default,   // button0
                               QMessageBox::NoButton,                    // button1
                               QMessageBox::NoButton );                  // button2
    }


    delete proc;

    lock=false;

}


void YQSaveLogs::SaveLogs()
{
    QString fileName = askForSaveFileName( "/tmp/y2logs.tgz",                   // startWith
                                           "*.tgz *.tar.gz",                    // filter
                                           "Save y2logs to..." );               // headline

    if ( ! fileName.isEmpty() )
    {
        QString saveLogsCommand = "/sbin/save_y2logs";
	QStringList args = QStringList() << fileName;

        if ( access( saveLogsCommand.toAscii(), X_OK ) == 0 )
        {
            lock = true;

            proc = new QProcess( this );

            connect ( proc, SIGNAL(finished( int, QProcess::ExitStatus )), this, 
		      SLOT(saveProcessExited( int, QProcess::ExitStatus )));

            proc->start( saveLogsCommand, args );
	    int result = proc->exitCode();

            if ( result != 0 )
            {
                // unable to start command (errors while running command are handled by saveProcessExited)

                QMessageBox::warning( 0,                                        // parent
                                      "Error",                                  // caption
                                      QString( "Couldn't save y2logs to %1 - "
                                               "exit code %2" ).arg( fileName ).arg( result ),
                                      QMessageBox::Ok | QMessageBox::Default,   // button0
                                      QMessageBox::NoButton,                    // button1
                                      QMessageBox::NoButton );                  // button2

		delete proc;

                lock = false;
            }
            else
            {
                emit statusMsg( _("Writing log files ...") );
            }
        }
        else
        {
            QMessageBox::warning( 0,                                            // parent
                                  "Error",                                      // caption
                                  QString( "Couldn't save y2logs to %1:\n"
                                           "Command %2 not found" ).arg( fileName ).arg( saveLogsCommand ),
                                  QMessageBox::Ok | QMessageBox::Default,       // button0
                                  QMessageBox::NoButton,                        // button1
                                  QMessageBox::NoButton );                      // button2
        }
    }
}

QString YQSaveLogs::askForSaveFileName( const QString & startWith,
                                  const QString & filter,
                                  const QString & headline )
{
    QString file_name;
    bool try_again = false;

    do
    {
        // Leave the mouse cursor alone - this function might be called from
        // some other widget, not only from AskForSaveFileName().

        file_name = QFileDialog::getSaveFileName( 0,			// parent
						  headline,		// caption
                                                  startWith,		// initial dir
                                                  filter );  		

        if ( file_name.isEmpty() )      // this includes file_name.isNull()
            return QString::null;


        if ( access( file_name.toAscii(), F_OK ) == 0 )    // file exists?
        {
            QString msg;

            if ( access(  file_name.toAscii(), W_OK ) == 0 )
            {
                // Confirm if the user wishes to overwrite an existing file
                msg = ( _( "%1 exists! Really overwrite?" ) ).arg( file_name );
            }
            else
            {
                // Confirm if the user wishes to overwrite a write-protected file %1
                msg = ( _( "%1 exists and is write-protected!\nReally overwrite?" ) ).arg( file_name );
            }

            int button_no = QMessageBox::information( 0,
                                                      // Window title for confirmation dialog
                                                      _( "Confirm"   ),
                                                      msg,
                                                      _( "C&ontinue" ),
                                                      _( "&Cancel"   ) );
            try_again = ( button_no != 0 );
        }

    } while ( try_again );

    return file_name;
}

#include <yqsavelogs.moc>
