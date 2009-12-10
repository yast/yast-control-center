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

#include "main_window.h"
#include "i18n.h"

#include <unistd.h>
#include <rpc/types.h>
#include <stdlib.h> 
#include <time.h>

#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QToolBar>
#include <QLineEdit>
#include <QDockWidget>
#include <QListView>
#include <QDebug>
#include <QQueue>
#include <QSettings>
#include <QStatusBar>
#include <QTimer>
#include <QMessageBox>

#include "kcategorizedsortfilterproxymodel.h"
#include "kcategorizedview.h"
#include "kcategorydrawer.h"
#include "yqmodulesproxymodel.h"

#include "yqmodulesmodel.h"
#include "yqmodulegroupsmodel.h"

//#include "moduleiconitem.h"
#define ORG_NAME "YaST2"
#define APP_NAME "y2controlcenter-qt"
#define USED_QUEUE_SIZE 5


/*
  Textdomain "control-center"
*/

class MainWindow::Private
{
public:
    Private()
        : modmodel(0L)
        , groupview(0L)
        , modview(0L)
        , kcsfpm(0L)
	, gcsfpm(0L)
	, lastCallTime(0)
	, minwait(5)
    {
    }
    ~Private()
     {
     }

    YQModulesModel *modmodel;
    QListView *groupview;
    KCategorizedView * modview;
    // category proxy model
    KCategorizedSortFilterProxyModel * kcsfpm;
    QSortFilterProxyModel *gcsfpm;

    QLineEdit *searchField;

    QQueue <QString>  recentlyUsed;

    QModelIndex lastCalledIndex;
    time_t lastCallTime;
    time_t minwait;

    bool noBorder;
    bool fullScreen;

};    

MainWindow::MainWindow( Qt::WindowFlags wflags )
  : QMainWindow( 0, wflags )
  , d(new Private)
{
    qDebug();

    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *layout = new QHBoxLayout;
    centralWidget->setLayout(layout);

    // setup central widget
    d->modview = new KCategorizedView( this );
    KCategoryDrawer * drawer = new KCategoryDrawer;

    layout->addWidget(d->modview);
    
    d->modview->setSelectionMode(QAbstractItemView::SingleSelection);
//        tv->setSpacing(KDialog::spacingHint());
    d->modview->setCategoryDrawer( drawer );
    d->modview->setViewMode( QListView::IconMode );
    d->modview->setItemDelegate( new ModuleIconItemDelegate( this , d->modview->style() ) );
    d->modview->setMouseTracking( true );
    d->modview->viewport()->setAttribute( Qt::WA_Hover );

    // init the models
    d->modmodel = new YQModulesModel(this);
    d->kcsfpm = new YQModulesProxyModel( this );
    d->kcsfpm->setCategorizedModel( true );
    d->kcsfpm->setSourceModel( d->modmodel) ;

    //kcsfpm->setFilterRole( KCModuleModel::UserFilterRole );
    //kcsfpm->setFilterCaseSensitivity( Qt::CaseInsensitive );
    d->kcsfpm->sort( 0 );
    d->modview->setModel( d->kcsfpm ); 

    // Setup Dock Widget with groups and search field
    QDockWidget *groupdock = new QDockWidget(this);
    groupdock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    QWidget *leftPanel = new QWidget( this );
    QVBoxLayout *leftPanelLayout = new QVBoxLayout( leftPanel );

    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel();
    d->searchField = new QLineEdit();
    searchLabel->setText( _("&Search") );
    searchLabel->setBuddy( d->searchField );
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(d->searchField);

    leftPanelLayout->addLayout( searchLayout );    

    d->gcsfpm = new QSortFilterProxyModel( this );
    d->gcsfpm->setSourceModel( d->modmodel->groupsModel() );
    d->gcsfpm->setFilterKeyColumn( 2 );
    d->gcsfpm->setFilterRole( Qt::UserRole );

    d->groupview = new QListView(  );
    d->groupview->setModel(d->gcsfpm);
    d->groupview->setIconSize( QSize(32,32) );

    //now pre-select something
    d->groupview->setSelectionMode( QAbstractItemView::SingleSelection );
    QModelIndex selection = d->groupview->model()->index(0,0);
    d->groupview->setCurrentIndex( selection );
    d->modview->selectCategory( d->modmodel->groupsModel()->data(selection).toString() );

    leftPanelLayout->addWidget( d->groupview );
    
    groupdock->setWidget( leftPanel );

    addDockWidget(Qt::LeftDockWidgetArea, groupdock);

    readSettings();

    initActions();
    addAction( shutdown );
    //d->modview->addAction( addToF );
    //d->modview->setContextMenuPolicy( Qt::ActionsContextMenu );

    setWinTitle();
    statusBar()->showMessage( _("Ready") );
    
    connect( d->groupview, SIGNAL( pressed( const QModelIndex & ) ),
             SLOT( slotGroupPressed( const QModelIndex & ) ) );

    connect( d->modview, SIGNAL( leftMouseClick ( const QModelIndex  &) ),
             SLOT( slotLaunchModule( const QModelIndex & ) ) );

    connect( d->modview, SIGNAL( activated( const QModelIndex & ) ),
             SLOT( slotLaunchModule( const QModelIndex & ) ) );

    connect( d->searchField, SIGNAL( textChanged( const QString &)),
	     SLOT( slotFilterChanged() ));

    connect( shutdown, SIGNAL( activated()), qApp, SLOT( quit()));

}

void MainWindow::setFullScreen( bool fs )
{
    d->fullScreen = fs;
}

void MainWindow::setNoBorder( bool nb )
{
    d->noBorder = nb ;
}

void MainWindow::initActions()
{
   //addToF = new QAction( "Add to Favourites", this ); 
   shutdown = new QAction( this );
   shutdown->setShortcut( ("Ctrl+Q") ); 
}

void MainWindow::slotGroupPressed( const QModelIndex &index )
{
    qDebug() << "Group Click";
    // scroll to the category in the main view
    QModelIndex modidx = d->modmodel->firstModuleInGroup(index);
    qDebug() << "Scroll to  :" << d->modmodel->groupsModel()->data(index).toString();
    qDebug() << "  1st item :" << d->modmodel->data(modidx).toString();
    if ( modidx.isValid() )
    {
	d->modview->selectCategory( d->modmodel->groupsModel()->data(index).toString() );
        d->modview->scrollTo(d->kcsfpm->mapFromSource(modidx));
    }
}

void MainWindow::slotModulePressed( const QModelIndex &index )
{
    // map the categorized index to the modules model index
    QModelIndex srcidx = d->kcsfpm->mapToSource(index);
    if ( ! srcidx.isValid() )
        return;
    
    qDebug() << "Module Click:" << d->modmodel->data(srcidx).toString();
    qDebug() << "-> " << srcidx.row() << " : " << d->modmodel->propertyValue(srcidx, "GenericName").toString();
}

void MainWindow::slotLaunchModule( const QModelIndex &index)
{
    if ( index == d->lastCalledIndex )
    {
            time_t now = time( NULL );

            // ignore accidential double clicks

            if ( now >= d->lastCallTime &&                 // make sure we not going backward in time
                                                        // (system time or time zone changed by
                                                        // some YaST2 module - see bug #71816)
                 now - d->lastCallTime < d->minwait )         // elapsed time too short?
            {
		qDebug() << "Ignoring 2nd click, clicking too fast" ;
                return; // ignore this click
            }
    }

    d->lastCallTime     = time( NULL );
    d->lastCalledIndex = index;

    QModelIndex i1 = d->modmodel->index( d->kcsfpm->mapToSource( index ).row(), YQDesktopFilesModel::Call );
    QModelIndex i2 = d->modmodel->index( d->kcsfpm->mapToSource( index ).row(), YQDesktopFilesModel::Argument );
    QModelIndex i3 = d->modmodel->index( d->kcsfpm->mapToSource( index ).row(), YQDesktopFilesModel::Name );

    if ( !i1.isValid() || !i2.isValid())
	return;

    QString client = d->modmodel->data( i1, Qt::UserRole ).toString();
    QString argument = d->modmodel->data( i2, Qt::UserRole ).toString();
    QString name = d->modmodel->data( i3, Qt::UserRole ).toString();

    QString cmd = QString("/sbin/yast2 ");
    cmd += client;

    if ( d->noBorder )	
	cmd += " --noborder ";
    if ( d->fullScreen )
	cmd += " --fullscreen ";

    if (!argument.isEmpty() )
    {
	cmd +=" '";
	cmd += argument;
	cmd +="'";
    }
    cmd += " &";

    //FIXME: use something more intelligent (unique) to remember used modules, names suck
    d->recentlyUsed.enqueue( name );  
    if( d->recentlyUsed.size() == USED_QUEUE_SIZE )
    {
	d->recentlyUsed.dequeue();
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    qDebug() << "Run command: " << cmd.toAscii();
    //Translators: module name comes here (%1) e.g. HTTP server, Scanner,...
    QString msg = _("Starting configuration module \"%1\"...").arg( name );
    statusBar()->showMessage( msg, 2000 );

    system( cmd.toAscii() ); 

    QTimer::singleShot( 3*1000, this, SLOT( slotRestoreCursor() ) ); 
}

void MainWindow::slotFilterChanged()
{
    QString stext = d->searchField->text();
    d->kcsfpm->bublisFilterFunction( stext );

    QString gr_filter = d->kcsfpm->matchingGroupFilterRegexp();
    d->gcsfpm->setFilterRegExp ( gr_filter );
}

void MainWindow::initialMsg()
{
    if ( !d->modmodel->isRoot() )
	QMessageBox::information(this, _("YaST Control Center"), 
            _("YaST Control Center is not running as root.\n"
	    "You will only see modules which do not require root privileges."));
}

void MainWindow::slotRestoreCursor()
{
    QApplication::restoreOverrideCursor();
}

void MainWindow::readSettings()
{
    QSettings settings(ORG_NAME, APP_NAME);
    
    settings.beginGroup("MainWindow");
    resize(settings.value("Size", QSize(680,420)).toSize()); 
    move(settings.value("Position", QPoint(200,200)).toPoint());
    settings.endGroup();

    settings.beginGroup("PersonalItems");
    QString used =  settings.value("RecentlyUsed").toString();
    QStringList used_list = used.split(",", QString::SkipEmptyParts);
    foreach( QString f, used_list )
    {
       d->recentlyUsed.enqueue( f );
    }
    settings.endGroup();

}

void MainWindow::writeSettings() 
{
    QSettings settings(ORG_NAME, APP_NAME);
     
    settings.beginGroup("MainWindow");
    settings.setValue("Size", size()); 
    settings.setValue("Position", pos());
    settings.endGroup();

    settings.beginGroup( "PersonalItems" ); 
    QStringList used_list( d->recentlyUsed ); 
    settings.setValue( "RecentlyUsed", used_list.join(",") );
    settings.endGroup();
}

void MainWindow::setWinTitle()
{
    QString title = _("YaST Control Center");
    char hostname[ MAXHOSTNAMELEN+1 ];
    if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
    {
	hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated

	if ( strlen( hostname ) > 0 && strcmp( hostname, "(none)" ) != 0 )
	{
	    title += " @ ";
	    title += hostname;
	}
    }
    setWindowTitle( title );
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    writeSettings();
    event->accept(); 
}

MainWindow::~MainWindow()
{
    delete d;
}

#include "main_window.moc"
