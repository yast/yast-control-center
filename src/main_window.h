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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "kcategorizedview.h"
#include "kcategorydrawer.h"
#include "moduleiconitem.h"
#include "yqsavelogs.h"


#include <QMainWindow>
#include <QMenu>
#include <QAction> 
#include <QCloseEvent>

/**
 * Main window of the control center
 *
 * @author Thomas Goettlicher <tgoettlicher@suse.de>
 */
class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  MainWindow( Qt::WindowFlags wflags = Qt::Window );
  ~MainWindow();

  void setFullScreen( bool fs); 
  void setNoBorder( bool nb ); 
  QMenu* createPopupMenu () { return NULL; };

protected slots:
  void slotGroupPressed( const QModelIndex &index );
  void slotModulePressed( const QModelIndex &index );
  void slotLaunchModule( const QModelIndex &index );
  void slotFilterChanged();
  void initialMsg();
  void slotRestoreCursor();

protected:
  void initActions();
  void closeEvent( QCloseEvent *event);
private:
  class Private;
  Private *const d;

  QAction *addToF;
  QAction *shutdown;
  QAction *saveLogs;

  YQSaveLogs *logSaver;

  void writeSettings();
  void readSettings();
  void setWinTitle();
  QSize readGroupViewSize();


  QDockWidget *groupdock;

};

#endif
