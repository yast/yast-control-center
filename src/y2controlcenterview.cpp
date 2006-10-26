/***************************************************************************
                  y2controlcenterview.cpp  -  listbox, iconview etc.
                             -------------------
    begin               : Mit Okt 18 14:21:09 CEST 2000
    Copyright		: (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
***************************************************************************/


/*
  Textdomain "control-center"
*/

#include "y2controlcenterview.h"
#include "y2cc_iconview.h"
#include "y2cc_globals.h"
#include "searchdialog.h"

#include <iostream>
// #include <qprogressbar.h>
#include <qmainwindow.h>
#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qhbox.h>
#include <unistd.h>
#include <stdlib.h>

using std::cout;
using std::cerr;
using std::endl;



#define SUPPRESS_LIST_BOX_HIGHLIGHT	1
#define USE_BUTTONS			0


#define LEFTLISTBOXWIDTH 200
//just guessed
#define MAXLEFTLISTBOXWIDTHDIFF 100
#define MAXLEFTLISTBOXHEIGHTDIFF 30

Y2ControlCenterView::Y2ControlCenterView(QWidget *parent)
    : QVBox(parent)
{
    _modules = 0;
    lastCalledModule = 0;
    lastCallTime = 0;
    minwait = 5;
    _searchDialog = 0;
    _items = 0;

    setMargin( 5 );

#if 0
    QWidget * title_bar = layoutTitleBar( this );
    CHECK_PTR( title_bar );
    title_bar->setMinimumWidth(640);
#endif

#if 0
    QFrame * separator = new QFrame( this );
    CHECK_PTR( separator );
    separator->setFrameStyle( QFrame::HLine | QFrame::Plain );
    layout->addWidget( separator );
#endif

    QHBox * contentHBox = new QHBox( this );


    //
    // Module Categories List
    //

    _listBox = new QListBox( contentHBox );
    _listBox->setBackgroundMode( PaletteDark );
    _listBox->setFixedWidth ( LEFTLISTBOXWIDTH );
    _listBox->setFrameStyle (QFrame::StyledPanel|QFrame::Raised);

    connect( _listBox,	SIGNAL( highlighted      ( int ) ),
	     this,	SLOT  ( slotButtonClicked( int ) ) );

    // change some colors to make it look nice
    QPalette pal( _listBox->palette() );

#if SUPPRESS_LIST_BOX_HIGHLIGHT
    pal.setColor( QColorGroup::Highlight,_listBox->palette().active().base() );
#endif
    pal.setColor( QColorGroup::Base,_listBox->palette().active().button() );
    pal.setColor( QColorGroup::HighlightedText,_listBox->palette().active().text() );
    _listBox->setPalette( pal );
    _listBox->setFocus();


    //
    // IconView for the Module Icons
    //

    _iconView = new MyQIconView( contentHBox );
    _iconView->setArrangement( QIconView::LeftToRight );
    _iconView->setResizeMode( QIconView::Adjust );
    _iconView->setGridX( 200 );
    // _iconView->setGridY( 40 );
    _iconView->setSpacing( 12 );
    _iconView->setItemTextPos( QIconView::Right );
    _iconView->setSorting( false );

    connect( _iconView,	SIGNAL( clicked		( QIconViewItem * ) ),
	     this,	SLOT  ( slotIconClicked	( QIconViewItem * ) ) );

    connect( _iconView, SIGNAL( returnPressed   ( QIconViewItem * ) ),
	     this,	SLOT  ( slotIconClicked ( QIconViewItem * ) ) );

    connect( _iconView, SIGNAL( onItem		( QIconViewItem * ) ),
	     this,	SLOT  ( slotOnItem	( QIconViewItem * ) ) );

    connect( _iconView,	SIGNAL( selectionChanged ( QIconViewItem *) ),
	     this, 	SLOT  ( slotOnItem	 ( QIconViewItem *) ) );


    //
    // Buttons
    //

    QHBox * buttonBox = new QHBox( this );

    QPushButton * button = 0;
    
#if USE_BUTTONS

    //
    // Help button
    //

    button = new QPushButton( _("&Help"), buttonBox );
    connect( button, SIGNAL( clicked() ), this, SLOT( help() ) );

#endif

    //
    // Search button
    //

    button = new QPushButton( _("&Search..."), buttonBox );
    connect( button, SIGNAL( clicked() ), this, SLOT( search() ) );


    // Stretcher
    QWidget * stretcher = new QWidget( buttonBox );
    stretcher->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert


#if USE_BUTTONS

    //
    // Close button
    //

    button = new QPushButton( _("&Close"), buttonBox );
    connect(button , SIGNAL( clicked() ), qApp, SLOT( quit() ) );
#endif

}


bool Y2ControlCenterView::init()
{
    _modules=new YModules();

    connect( _modules,	SIGNAL( modulesReady    () ),
	     this,	SLOT  ( slotInitListBox () ) );

    connect( _modules, 	SIGNAL( modError   ( QString ) ),
	     this, 	SLOT  ( errorPopup ( QString ) ) );

    QApplication::setOverrideCursor( Qt::waitCursor );

    if ( ! _modules->init() )
    {
	QApplication::restoreOverrideCursor();
	error=_modules->getErrorString();
	return false;
    }
    return true;
}



// Stolen from YQWizard

QWidget * Y2ControlCenterView::layoutTitleBar( QWidget * parent )
{
    QPixmap titleBarGradientPixmap = QPixmap( PIXMAP_DIR "/title-bar-gradient.png" );

    QHBox * titleBar = new QHBox( parent );
    CHECK_PTR( titleBar );
    setGradient( titleBar, titleBarGradientPixmap );
    titleBar->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert

    //
    // Left logo
    //

    QLabel * left = new QLabel( titleBar );
    left->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    QPixmap leftLogo( PIXMAP_DIR "/title-bar-left.png" );

    if ( ! leftLogo.isNull() )
    {
	left->setPixmap( leftLogo );
	left->setFixedSize( leftLogo.size() );
	left->setBackgroundOrigin( QWidget::ParentOrigin );
    }


    //
    // Center stretch space
    //

    QWidget * spacer = new QWidget( titleBar );
    CHECK_PTR( titleBar );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert


    //
    // Right logo
    //

    QLabel * right = new QLabel( titleBar );
    CHECK_PTR( right );

    QPixmap rightLogo( PIXMAP_DIR "/title-bar-right.png" );

    if ( ! rightLogo.isNull() )
    {
	right->setPixmap( rightLogo );
	right->setFixedSize( rightLogo.size() );
	right->setBackgroundOrigin( QWidget::ParentOrigin );
    }

    return titleBar;
}


// Stolen from YQWizard

void Y2ControlCenterView::setGradient( QWidget * widget, const QPixmap & pixmap )
{
    if ( widget && ! pixmap.isNull() )
    {
	widget->setFixedHeight( pixmap.height() );
	widget->setPaletteBackgroundPixmap( pixmap );
    }
}



void Y2ControlCenterView::errorPopup( QString msg )
{
    QApplication::setOverrideCursor( Qt::arrowCursor );
    QMessageBox::warning( this, _("YaST2 Control Center"),msg);
    QApplication::restoreOverrideCursor();
}


void Y2ControlCenterView::slotInitListBox()
{
    QString icon,groupname;
    QString icondir = ICON_DIR "/";
    int i=0,firstenabled=-1;
    QListBoxPixmap* pixmap;

//  _modules->dumpgroups();

    //walk through groups and insert icons for them into listbox
    for ( const ModGroup* ptr=_modules->firstGroup();ptr;ptr=_modules->nextGroup(),i++)
    {
	icon=ptr->getIcon();
	groupname=ptr->getName();
	pixmap=new QListBoxPixmap(QPixmap(icondir + icon),groupname);
	_listBox->insertItem(pixmap);
	if (ptr->isEmpty())
	{
	    pixmap->setSelectable(false);
	}
	else if (firstenabled<0)
	{
	    firstenabled=i;
	}
    }

    _listBox->setSelected(firstenabled<0?0:firstenabled,true);

    //we don't want scrollbars if possible
    QTimer::singleShot( 0, this, SLOT( slotAdjustListbox() ) );

    emit statusMsg( _("Ready.") );
    QApplication::restoreOverrideCursor();


    //we currently only need a config file if we are not root
    if (config.isroot == false)
    {
	QDir homedir=QDir::home();
	QFile configfile;
	bool dirok=true;
	//cd to ~/.yast2 otherwise try to create it
	if ( !homedir.cd(".yast2"))
	{
	    dirok=false;
	    if (QFile::exists(".yast2"))
	    {
		//something with that name exist, but we
		//cannot cd into it
	    }
	    else
	    {
		if (!homedir.mkdir(".yast2"))
		{
		    //this didn't work too
		}
		else if ( homedir.cd(".yast2"))
		{
		    dirok=true;
		}
	    }
	}
	configfile.setName(homedir.absFilePath("y2controlcenterrc"));
	//only display msg if file does not exist
	//TODO: write a real config file
	if (!configfile.exists())
	{
	    if (dirok)
	    {
		configfile.open(IO_ReadWrite);
	    }
	    QMessageBox::information( this, _("YaST2 Control Center"),
				      _("YaST2 Control Center is not running as root.\n"
					"You will only see modules which do not require root privileges"));
	}
    }
}

void Y2ControlCenterView::slotAdjustListbox()
{
    int width = _listBox->contentsWidth() - _listBox->visibleWidth();

    if ( width > 0 )
    {
	if ( width > MAXLEFTLISTBOXWIDTHDIFF )
	{
	    width = MAXLEFTLISTBOXWIDTHDIFF;
	}
	width +=_listBox->width();
	width += 2;	// extra margin
	_listBox->setFixedWidth( width );
	_listBox->updateGeometry();
    }

    int height =_listBox->contentsHeight() - _listBox->visibleHeight();

    if ( height > 0 )
    {
	if ( height > MAXLEFTLISTBOXHEIGHTDIFF )
	{
	    height = MAXLEFTLISTBOXHEIGHTDIFF;
	}
	height += _listBox->height();
	_listBox->setMinimumHeight( height );
	_listBox->updateGeometry();
    }
}

Y2ControlCenterView::~Y2ControlCenterView()
{
    delete _searchDialog;
    delete _items;
    delete _modules;
}

const QString* Y2ControlCenterView::getErrorString() const
{
    return error;
}

void Y2ControlCenterView::slotIconClicked(QIconViewItem *item)
{
    if ( item )
	runModule( ( (MyQIconViewItem *) item )->getModule() );
}


void Y2ControlCenterView::runModule( const YMod *mod )
{
    if ( mod )
    {
	if ( mod == lastCalledModule )
	{
	    time_t now = time( NULL );

	    // ignore accidential double clicks


	    if ( now >= lastCallTime &&			// make sure we not going backward in time
	    						// (system time or time zone changed by
							// some YaST2 module - see bug #71816)
		 now - lastCallTime < minwait )		// elapsed time too short?
	    {
		return;	// ignore this click
	    }
	}

	lastCallTime	 = time( NULL );
	lastCalledModule = mod;

	QApplication::setOverrideCursor( Qt::waitCursor );

	QString msg= _("Starting module %1...").arg( mod->getName());
	emit statusMsg( msg);

	_modules->runModule( mod );

	QTimer::singleShot( 3*1000, this, SLOT( slotResetCursor() ) );
    }
    else
    {
	cerr << "pointer is 0 in file: " << __FILE__ << ", line: " << __LINE__ << endl;
    }
}

void Y2ControlCenterView::slotOnItem(QIconViewItem *item)
{
    if (!item) return;

    const YMod* m=((MyQIconViewItem*)item)->getModule();

    if (!m) return;

    const QString& s=m->getDescription();
    emit statusMsg(s);
}


void
Y2ControlCenterView::fillIconView( int group_id )
{
    _iconView->clear ();

    ModGroup* group = _modules->setGroup( group_id );

    if ( !group )
	return;

    const YMod* m = group->first ();
    while (m)
    {
	QString iconfile = ICON_DIR "/";
	iconfile += m->getIcon ();

	QPixmap pixmap (iconfile);
#if 0
	if (pixmap.isNull ())
	    qDebug ("failed to load icon %s", (const char*) iconfile);
#endif

	MyQIconViewItem * icon = new MyQIconViewItem (_iconView, m->getName (), pixmap);
	icon->setModule (m);
	icon->setDragEnabled (TRUE);

	m = group->next ();
    }
}


void Y2ControlCenterView::slotButtonClicked(int id)
{
    fillIconView( id );
}


void Y2ControlCenterView::slotResetCursor()
{
    QApplication::restoreOverrideCursor();
}


void Y2ControlCenterView::help()
{
    // title of help message box with help text
    QString titletext=_("YaST2 Control Center - Help");
    // button for message box with help text
    QString buttontext=_("Ok");
    //TODO: insert help text
    // the helptext it self
    QString helptext=_("By clicking on icons to the left, you can choose the type of\n"
		       "configuration modules (e.g. Hardware or Software) to be displayed.\n\n")
	+ _("To run one of the modules on the right side, simply single-click on the\n"
	    "corresponding icon. \n\n"
	    "Be patient, starting of modules can take some seconds.\n\n")
	+ _("You can also search for keywords (e.g. modem) by clicking on the \"Search\" button");
    QMessageBox::information(this,titletext,helptext,buttontext);
}



void Y2ControlCenterView::search()
{
    if ( ! _items )
	_items= new QVector<const YMod>;

    if (! _searchDialog )
    {
	_searchDialog = new SearchDialog(this);
	_searchDialog->resize(380,380);
	connect( _searchDialog,	SIGNAL( sigSearch       ( QString ) ),
		 this,		SLOT  ( slotSearchModule( QString ) ) );
    }

    int searchDialogResult = _searchDialog->exec();

    // item clicked (0="ok", 1="cancel" -> ok-2 = module to run)
    if ( searchDialogResult > 1 )
    {
	runModule( _items->at( searchDialogResult - 2 ) );
    }
}

void Y2ControlCenterView::slotSearchModule(QString text)
{
    if ( ! _items )
	return;

    _items->clear();
    QString icondir = ICON_DIR "/";
    if (!text.isEmpty())
    {
	_searchDialog->ClearResults();
	const YMod* m;
	unsigned int i=0;

	//walk through all groups

	for ( ModGroup* ptr=_modules->firstGroup();ptr;ptr=_modules->nextGroup())
	{
	    m=ptr->first();
	    //walk through all modules in group
	    while (m)
	    {
		if (m->getName().contains(QRegExp(text,false,false)) || \
		    m->getDescription().contains(QRegExp(text,false,false)))
		{
//		    cout << "Found: " << m->getName() << "(" <<i  << ")" << endl;
		    _searchDialog->SearchResult(new QListBoxPixmap(QPixmap(icondir + m->getIcon()),m->getName()));
		    if (_items->size()<=i)
		    {
			_items->resize(i+1);
		    }
		    _items->insert(i,m);
		    i++;
		}
		m=ptr->next();
	    }
	}
	if (i==0)
	{
	    _searchDialog->message(_("Nothing found"),10000);
	}
	else
	{
	    // print search result
	    QString msg=_("%1 module found","%1 modules found",i);
	    msg=msg.arg(i);
	    _searchDialog->message(msg,10000);
	}
    }
    else
    {
	_searchDialog->ClearResults();
	_searchDialog->message(_("No search string specified"),10000);
    }

}

#include <y2controlcenterview.moc.cpp>
