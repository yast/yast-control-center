/***************************************************************************
                 y2cc_iconview.cpp  -  reimplements QIconView
                             -------------------
    begin               : 28.05.2001
    Copyright		: (c) 2001 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/


#include "y2cc_iconview.h"

#define SBINYAST2 "/sbin/yast2"

//static char *cvs_id = "$Id$";

// *
// * MyQIconViewItem
// *

MyQIconViewItem::MyQIconViewItem ( QIconView * parent, const QString & text, const QPixmap & icon ):QIconViewItem(parent,text,icon)
{
}

MyQIconViewItem::~MyQIconViewItem()
{
}


// *
// * Y2CCIconDrag
// *

#ifdef WITHICONMOVE
/*
Y2CCIconDrag::Y2CCIconDrag( QWidget * dragSource, const char* name = 0 )
 : QIconDrag( dragSource, name )
{
}

const char* Y2CCIconDrag::format( int i ) const
{
    if ( i == 0 )
	return "application/x-qiconlist";
    else if ( i == 1 )
//	return "text/uri-list";
	return "text/plain";
    else
	return 0;
}

void Y2CCIconDrag::addFile( const QString& file )
{
	filenames+=file+"\n";
}

QByteArray Y2CCIconDrag::encodedData( const char* mime ) const
{
    QByteArray a;
    if ( QString( mime ) == "application/x-qiconlist" ) {
	a = QIconDrag::encodedData( mime );
//    } else if ( QString( mime ) == "text/uri-list" ) {
    } else if ( QString( mime ) == "text/plain" ) {
	QString s = filenames;
	a.resize( s.length() );
	memcpy( a.data(), s.latin1(), s.length() );
    }
    return a;
}
*/
#endif

// *
// * MyQIconView
// *


MyQIconView::MyQIconView( QWidget * parent, const char * name, WFlags f ) :
	QIconView( parent , name , f )
{
	setItemsMovable(false);
}

MyQIconView::~MyQIconView()
{
}

QDragObject * MyQIconView::dragObject()
{
    if ( !currentItem() )
	return 0;

#ifdef WITHICONMOVE
/*
    QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
    Y2CCIconDrag *drag = new Y2CCIconDrag( viewport() );
    drag->setPixmap( *currentItem()->pixmap(),
 		     QPoint( currentItem()->pixmapRect().width() / 2, currentItem()->pixmapRect().height() / 2 ) );
    for ( MyQIconViewItem *item = (MyQIconViewItem*)firstItem(); item;
	  item = (MyQIconViewItem*)item->nextItem() ) {
	if ( item->isSelected() ) {
	    QIconDragItem id;
	    id.setData( QCString( "isthiseverused?" ) );
	    if (item->getModule())
	    {
		    QString desktopentry="[Desktop Entry]\n"
					"Exec=" SBINYAST2 " %1\n"
					"Icon=" ICONDIR "/" "%2\n"
					"Type=Application\n"
					"Name=%3\n"
					"X-KDE-SubstituteUID=true\n"
					"X-KDE-Username=root\n" ;
		desktopentry=desktopentry.arg(item->getModule()->getYCPName());
		desktopentry=desktopentry.arg(item->getModule()->getIcon());
		desktopentry=desktopentry.arg(item->getModule()->getName());
//	    	drag->addFile(item->getModule()->getName());
	    	drag->addFile(desktopentry);
	    }
	    drag->append( id,
			  QRect( item->pixmapRect( FALSE ).x() - orig.x(),
				 item->pixmapRect( FALSE ).y() - orig.y(),
				 item->pixmapRect().width(), item->pixmapRect().height() ),
			  QRect( item->textRect( FALSE ).x() - orig.x(),
				 item->textRect( FALSE ).y() - orig.y(),
				 item->textRect().width(), item->textRect().height() )
			   );
	}
    }
*/
#else
    MyQIconViewItem *item = (MyQIconViewItem*)currentItem();
    QTextDrag *drag=0L;
    if (item)
    {
	if (item->getModule())
	{
		drag = new QTextDrag( viewport() );
		drag->setSubtype("plain");
		drag->setPixmap( *currentItem()->pixmap(),
		QPoint( currentItem()->pixmapRect().width() / 2,
			currentItem()->pixmapRect().height() / 2 ) );
		QString desktopentry="[Desktop Entry]\n"
					"Exec=" SBINYAST2 " %1\n"
					"Icon=" ICONDIR "/" "%2\n"
					"Type=Application\n"
					"Name=%3\n"
					"X-KDE-SubstituteUID=true\n"
					"X-KDE-Username=root\n" ;
		desktopentry=desktopentry.arg(item->getModule()->getYCPName());
		desktopentry=desktopentry.arg(item->getModule()->getIcon());
		desktopentry=desktopentry.arg(item->getModule()->getName());
		drag->setText(desktopentry);
	}
    }
#endif

    return drag;
}

#include <y2cc_iconview.moc.cpp>
