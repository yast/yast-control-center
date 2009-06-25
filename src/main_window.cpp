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

#include <QLayout>
#include <QLabel>
#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QDockWidget>
#include <QListView>
#include <QDebug>
#include <QQueue>
#include <QSettings>
#include <QStatusBar>

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

class MainWindow::Private
{
public:
    Private()
        : modmodel(0L)
        , groupview(0L)
        , modview(0L)
        , kcsfpm(0L)
	, gcsfpm(0L)
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
};    

MainWindow::MainWindow()
  : QMainWindow()
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
    QWidget *leftPanel = new QWidget( this );
    QVBoxLayout *leftPanelLayout = new QVBoxLayout( leftPanel );

    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel();
    searchLabel->setText( _("Search") );
    searchLayout->addWidget(searchLabel);
    d->searchField = new QLineEdit();
    searchLayout->addWidget(d->searchField);

    leftPanelLayout->addLayout( searchLayout );    

    d->gcsfpm = new QSortFilterProxyModel( this );
    d->gcsfpm->setSourceModel( d->modmodel->groupsModel() );
    d->gcsfpm->setFilterKeyColumn( 2 );
    d->gcsfpm->setFilterRole( Qt::UserRole );

    d->groupview = new QListView(  );
    d->groupview->setModel(d->gcsfpm);
    d->groupview->setIconSize( QSize(32,32) );
    leftPanelLayout->addWidget( d->groupview );
    
    groupdock->setWidget( leftPanel );

    addDockWidget(Qt::LeftDockWidgetArea, groupdock);


    readSettings();

    initActions();
    d->modview->addAction( addToF );
    d->modview->setContextMenuPolicy( Qt::ActionsContextMenu );

    setWinTitle();
    statusBar()->showMessage( _("Ready") );
    
    connect( d->groupview, SIGNAL( pressed( const QModelIndex & ) ),
             SLOT( slotGroupPressed( const QModelIndex & ) ) );

    connect( d->modview, SIGNAL( pressed( const QModelIndex & ) ),
             SLOT( slotModulePressed( const QModelIndex & ) ) );

    connect( d->modview, SIGNAL( doubleClicked( const QModelIndex & ) ),
             SLOT( slotLaunchModule( const QModelIndex & ) ) );

    connect( d->searchField, SIGNAL( textChanged( const QString &)),
	     SLOT( slotFilterChanged() ));

}

void MainWindow::initActions()
{
   addToF = new QAction( "Add to Favourites", this ); 
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

    qDebug() << "Run command: " << cmd.toAscii();
    QString msg = _("Starting module %1...").arg( client );
    statusBar()->showMessage( msg, 2000 );

    system( cmd.toAscii() ); 
}

void MainWindow::slotFilterChanged()
{
    QString stext = d->searchField->text();
    d->kcsfpm->bublisFilterFunction( stext );

    QString gr_filter = d->kcsfpm->matchingGroupFilterRegexp();
    d->gcsfpm->setFilterRegExp ( gr_filter );
}

void MainWindow::readSettings()
{
    QSettings settings(ORG_NAME, APP_NAME);
    
    settings.beginGroup("MainWindow");
    resize(settings.value("Size", QSize(680,420)).toSize()); 
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
    settings.endGroup();

    settings.beginGroup( "PersonalItems" ); 
    QStringList used_list( d->recentlyUsed ); 
    settings.setValue( "RecentlyUsed", used_list.join(",") );
    settings.endGroup();
}

void MainWindow::setWinTitle()
{
    QString title = _("YaST2 Control Center");
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
