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
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtimer.h>
#include <unistd.h>
#include <stdlib.h>

using std::cout;
using std::cerr;
using std::endl;

//static char *cvs_id = "$Id$";

#define LEFTLISTBOXWIDTH 200
//just guessed
#define MAXLEFTLISTBOXWIDTHDIFF 100
#define MAXLEFTLISTBOXHEIGHTDIFF 30

Y2ControlCenterView::Y2ControlCenterView(QWidget *parent) : QWidget(parent)
{
  QPushButton *p;
//  QVBoxLayout *buttons;
  QLabel *suselogo;
  mods=0L;
  error=0L;
  lastmodule=0L;
  lasttime=0;
  minwait=5;
  sd=0L;
  items=0L;
  icons=0L;

  // connect(qApp, SIGNAL(aboutToQuit()), mods, SLOT(startPreparedY2("terminateY2Base")));

  //layout: vbox with iconfilepixmap and hbox
  layout = new QVBoxLayout(this);


  suselogo= new QLabel(this);
  suselogo->setPixmap(QPixmap(ICONDIR "/heading03.png"));
  suselogo->setMinimumWidth(640);
  layout->addWidget(suselogo);

  hlayout = new QHBoxLayout(layout);
  hlayout->setMargin(15);

  buttonhlayout= new QHBoxLayout(layout);
  buttonhlayout-> setMargin(15);

  p=new QPushButton( _("&Help"), this );
  connect(p, SIGNAL(clicked()), this, SLOT(slothelp()));
  buttonhlayout->addWidget(p);
  buttonhlayout-> addSpacing(10);
// the Search Button
  p=new QPushButton( _("&Search"), this );
  connect(p, SIGNAL(clicked()), this, SLOT(slotsearch()));
  buttonhlayout->addWidget(p);
  buttonhlayout-> addStretch();
// Button label
  p=new QPushButton( _("&Close"), this );
  connect(p, SIGNAL(clicked()), this, SLOT(slotquit()));
  // QToolTip::add(p,"");
  buttonhlayout->addWidget(p);

  //listbox
  listbox=new QListBox(this);
  listbox->setBackgroundMode(PaletteDark);
  listbox->setFixedWidth ( LEFTLISTBOXWIDTH );
  listbox->setFrameStyle (QFrame::StyledPanel|QFrame::Raised);
  connect(listbox, SIGNAL(highlighted(int)), this, SLOT(slotButtonClicked(int)));
//  listbox->setBackgroundColor(QWidget::gray);

// change some colors to make it look nice
  QPalette pal(listbox->palette());
  pal.setColor(QColorGroup::Highlight,listbox->palette().active().base());
  pal.setColor(QColorGroup::Base,listbox->palette().active().button());
  pal.setColor(QColorGroup::HighlightedText,listbox->palette().active().text());
//  pal.setColor(QColorGroup::Base,"grey");
//  pal.setColor(QColorGroup::HighlightedText,"black");
  listbox->setPalette(pal);
  listbox->setFocus();

  hlayout->addWidget(listbox);

  //iconview
  icons= new MyQIconView(this);
  icons->setArrangement(QIconView::LeftToRight);
//  icons->setAutoArrange(true);
  icons->setResizeMode(QIconView::Adjust);
  icons->setSpacing(20);
  icons->setGridX(200);
  icons->setGridY(60);
  icons->setItemTextPos(QIconView::Right);
  icons->setSorting(false);
  connect(icons, SIGNAL(clicked (QIconViewItem *)), this, SLOT(slotIconClicked(QIconViewItem *)));
  connect(icons, SIGNAL(returnPressed (QIconViewItem *)), this, SLOT(slotIconClicked(QIconViewItem *)));
  connect(icons, SIGNAL(onItem (QIconViewItem *)), this, SLOT(slotOnItem(QIconViewItem *)));
  connect(icons, SIGNAL(selectionChanged (QIconViewItem *)), this, SLOT(slotOnItem(QIconViewItem *)));
  hlayout->addWidget(icons);
}

bool Y2ControlCenterView::init()
{
  mods=new YastModules();

  connect(mods, SIGNAL(sig_percentread(int)), this, SLOT(slot_percentread(int)));
  connect(mods, SIGNAL(sig_finished(int)), this, SLOT(slotInitListbox(int)));
  connect(mods, SIGNAL(sig_error(QString)), this, SLOT(errorpopup(QString)));

  QApplication::setOverrideCursor( Qt::waitCursor );

  if (!mods->init())
  {
	  QApplication::restoreOverrideCursor();
	  error=mods->getErrorString();
	  return false;
  }
  return true;
}

void Y2ControlCenterView::errorpopup(QString msg)
{
	QApplication::setOverrideCursor( Qt::arrowCursor );
	QMessageBox::warning( this, _("YaST2 Control Center"),msg);
	QApplication::restoreOverrideCursor();
}

void Y2ControlCenterView::slotInitListbox(int code)
{
  QString icon,groupname;
  QString icondir = ICONDIR "/";
  int i=0,firstenabled=-1;
  QListBoxPixmap* pixmap;

//  mods->dumpgroups();

  //walk through groups and insert icons for them into listbox
  for ( const ModGroup* ptr=mods->firstGroup();ptr;ptr=mods->nextGroup(),i++)
  {
	icon=ptr->getIcon();
	groupname=ptr->getName();
	pixmap=new QListBoxPixmap(QPixmap(icondir + icon),groupname);
	listbox->insertItem(pixmap);
	if(ptr->isEmpty())
	{
		pixmap->setSelectable(false);
	}
	else if(firstenabled<0)
	{
		firstenabled=i;
	}
  }

  listbox->setSelected(firstenabled<0?0:firstenabled,true);

	//we don't want scrollbars if possible
	QTimer::singleShot(0,this, SLOT(slotAdjustListbox()));

//	emit sig_percentread(100);
	emit statusmsg(_("Ready."));
	QApplication::restoreOverrideCursor();

	//get_modulelist.ycp failed for some reason
	if(code)
	{
		this->errorpopup(_("There was an error while attempting to "
			  "obtain a list of available modules.\n"
			  "The list of displayed modules may be incomplete.\n\n"
			  "Please make sure that YaST2 and YaST2 Control "
			  "Center are installed properly"));
	}

	//we currently only need a config file if we are not root
	if(config.isroot == false)
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
				if(!homedir.mkdir(".yast2"))
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
		if(!configfile.exists())
		{
			if(dirok)
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
	// check if there's hidden content and resize
	int listboxwidth =listbox->contentsWidth()-listbox->visibleWidth();
	int morewidthtolooknice = 2;
	if(listboxwidth>0)
	{
//		clog << "width: " << listboxwidth << endl;
		if(listboxwidth>MAXLEFTLISTBOXWIDTHDIFF)
		{
			listboxwidth=MAXLEFTLISTBOXWIDTHDIFF;
		}
		listboxwidth+=listbox->width()+morewidthtolooknice;
//		clog << "width: " << listboxwidth << endl;
		listbox->setFixedWidth(listboxwidth);
		listbox->updateGeometry();
	}
	int listboxheight =listbox->contentsHeight()-listbox->visibleHeight();
	if(listboxheight>0)
	{
		if(listboxheight>MAXLEFTLISTBOXHEIGHTDIFF)
		{
			listboxheight=MAXLEFTLISTBOXHEIGHTDIFF;
		}
		listboxheight+=listbox->height();
//		clog << "height: " << listbox->height() << endl;
		listbox->setMinimumHeight(listboxheight);
		listbox->updateGeometry();
	}
}

Y2ControlCenterView::~Y2ControlCenterView()
{
	delete sd;
	sd=0L;
	delete items;
	items=0L;
	delete mods;
}

const QString* Y2ControlCenterView::getErrorString() const
{
	return error;
}

void Y2ControlCenterView::slotIconClicked(QIconViewItem *item)
{
	if(item)
	{
//		cout << "Icon Nr. " << item->index() << " clicked" << endl;
		this->runModule(((MyQIconViewItem*)item)->getModule());
	}
	else
	{
//		cout << "In die Prärie geklickt" << endl;
	}
}

void Y2ControlCenterView::slot_percentread(int percent)
{
	emit sig_percentread(percent);
}

// run Module m only if at least minwait seconds have
// passed to prevend accidential double clicks
void Y2ControlCenterView::runModule(const YastModule *m)
{
		time_t now;

		if(m)
		{
			if( (now=time(NULL))-lasttime<minwait && m==lastmodule )
			{
				return;
			}
			lasttime=now;
			lastmodule=m;

			QApplication::setOverrideCursor( Qt::waitCursor );

//			cout << "Calling " << m->getYCPName() << endl;
			QString s=_("calling module %1 ...");
			s=s.arg(m->getName());
			emit statusmsg(s);
			mods->runModule(m);
			QTimer::singleShot(3*1000,this, SLOT(slotresetcursor()));
		}
		else
		{
			cerr << "pointer is 0 in file: " << __FILE__ << ", line: " << __LINE__ << endl;
		}

}

void Y2ControlCenterView::slotOnItem(QIconViewItem *item)
{
	if(!item) return;

	const YastModule* m=((MyQIconViewItem*)item)->getModule();

	if(!m) return;

	const QString& s=m->getDesc();
	emit statusmsg(s);
}


void
Y2ControlCenterView::filliconview (int groupnr)
{
    icons->clear ();

    ModGroup* group = mods->setGroup (groupnr);
    if (!group)
	return;

    const YastModule* m = group->first ();
    while (m)
    {
	QString iconfile = ICONDIR "/";
	iconfile += m->getIcon ();

	QPixmap pixmap (iconfile);
#if 0
	if (pixmap.isNull ())
	    qDebug ("failed to load icon %s", (const char*) iconfile);
#endif

	MyQIconViewItem* icon = new MyQIconViewItem (icons, m->getName (), pixmap);
	icon->setModule (m);
	icon->setDragEnabled (TRUE);

	m = group->next ();
    }
}


void Y2ControlCenterView::slotButtonClicked(int id)
{
	filliconview(id);
}

void Y2ControlCenterView::slotquit()
{
	emit quit();
}
void Y2ControlCenterView::slotresetcursor()
{
	QApplication::restoreOverrideCursor();
}
void Y2ControlCenterView::slothelp()
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
void Y2ControlCenterView::slotsearch()
{
	unsigned int ok = 1;
	if(!items)
	{
		items= new QVector<const YastModule>;
	}
	if(!sd)
	{
		sd = new SearchDialog(this);
		sd->resize(380,380);
		connect(sd, SIGNAL(sigsearch(QString)), this, SLOT(slotsearchmodule(QString)));
	}
	ok=sd->exec();

	// item clicked (0="ok", 1="cancel" -> ok-2 = module to run)
	if(ok>1)
	{
		runModule(items->at(ok-2));
	}
}

void Y2ControlCenterView::slotsearchmodule(QString text)
{
		items->clear();
		QString icondir = ICONDIR "/";
		if(!text.isEmpty())
		{
//			cerr << "Searching for " << text << endl;
			sd->ClearResults();
			const YastModule* m;
			unsigned int i=0;
			//walk through all groups
			for ( ModGroup* ptr=mods->firstGroup();ptr;ptr=mods->nextGroup())
			{
				m=ptr->first();
				//walk through all modules in group
				while(m)
				{
					if (m->getName().contains(QRegExp(text,false,false)) || \
						m->getDesc().contains(QRegExp(text,false,false)))
					{
//						cout << "Found: " << m->getName() << "(" <<i  << ")" << endl;
						sd->SearchResult(new QListBoxPixmap(QPixmap(icondir + m->getIcon()),m->getName()));
						if(items->size()<=i)
						{
							items->resize(i+1);
						}
						items->insert(i,m);
						i++;
					}
					m=ptr->next();
				}
			}
			if(i==0)
			{
				sd->message(_("Nothing found"),10000);
			}
			else
			{
				// print search result
				QString msg=_("%1 module found","%1 modules found",i);
				msg=msg.arg(i);
				sd->message(msg,10000);
			}
		}
		else
		{
			sd->ClearResults();
			sd->message(_("No search string specified"),10000);
		}

}

#include <y2controlcenterview.moc.cpp>
