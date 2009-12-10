/*---------------------------------------------------------------------\
|                                                                      |
|                        __   __    _____ _____                        |
|                        \ \ / /   /  ___|_   _|                       |
|                         \ V /__ _\ `--.  | |                         |
|                          \ // _` |`--. \ | |                         |
|                          | | (_| /\__/ / | |                         |
|                          \_/\__,_\____/  \_/                         |
|                                                                      |
\---------------------------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "kcategorizedview.h"
#include "kcategorydrawer.h"
#include "moduleiconitem.h"


#include <QMainWindow>
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

  void writeSettings();
  void readSettings();
  void setWinTitle();

};

#endif
