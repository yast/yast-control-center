/***************************************************************************
                              searchdialog.cpp
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    Copyright		: (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/


/*
  Textdomain "control-center"
*/

#include "searchdialog.h"
#include "myintl.h"


SearchDialog::SearchDialog(QWidget *parent) :
	QDialog(parent,0,true)
{
	QPushButton *search,*cancel;
	QLabel *label;
	QHBoxLayout *hbox;
	lb=0L;
	vbox=0L;
	sb=0L;
	// Searchdialog
	label=new QLabel( _("Please enter keywords:"), this );
	// button
	search=new QPushButton( _("&Search"), this );
	search->setDefault(true);
	connect(search, SIGNAL(clicked()), this, SLOT(slotsearch()));
	// button
	cancel=new QPushButton( _("&Cancel"), this );
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	txt=new QLineEdit( this );
	txt->setFocus();
	connect(txt, SIGNAL(returnPressed()), this, SLOT(slotsearch()));
	vbox=new QVBoxLayout(this);
	vbox->addWidget(label);
	vbox->addSpacing(2);
	vbox->addWidget(txt);
	vbox->addSpacing(3);
	vbox->setMargin(10);
	hbox=new QHBoxLayout(vbox);
	hbox->setSpacing(5);
	hbox->addStretch(1);
	hbox->addWidget(search);
	lb=new QListBox(this);
	connect(lb, SIGNAL(clicked(QListBoxItem *)), this, SLOT(slotbye(QListBoxItem *)));
	connect(lb, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(slotbye(QListBoxItem *)));
	vbox->addSpacing(13);
	vbox->addWidget(lb);
	hbox->addWidget(cancel);

	// create statusbar
	sb=new QStatusBar(this);
	sb->setSizeGripEnabled(false);
	vbox->addWidget(sb);
	setCaption(_("YaST2 Control Center - Search"));
}

SearchDialog::~SearchDialog()
{
	delete sb;
	delete lb;
	delete txt;
	delete vbox;
}

int SearchDialog::exec()
{
	txt->setFocus();
	txt->selectAll();
	return QDialog::exec();
}

QString SearchDialog::getText() const
{
	qDebug("*** obsolete getText called ***");
	return txt->text();
}

void SearchDialog::SearchResult ( QListBoxItem * item )
{
	if(item)
		lb->insertItem(item);
}

void SearchDialog::ClearResults()
{
	if(lb)
	{
		lb->clear();
	}
}

void SearchDialog::message(const QString & msg,int time)
{
	if(!time)
	{
		sb->message(msg);
	}
	else
	{
		sb->message(msg,time);
	}
}

void SearchDialog::slotbye(QListBoxItem *item)
{
	int num=lb->index(item);
	if(num<0)
	{
		return;
	}
	else
	{
		done(num+2);
	}
}

void SearchDialog::slotsearch()
{
	emit sigsearch(txt->text());
}

#include <searchdialog.moc.cpp>
