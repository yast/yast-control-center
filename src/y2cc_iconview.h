/***************************************************************************
                            y2cc_iconview.h
                             -------------------
    begin               : 28.05.2001
    Copyright           : (c) 2001 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef Y2CC_ICONVIEW_H
#define Y2CC_ICONVIEW_H

#include <qiconview.h>
#include <qlistbox.h>
#include "yastmodules.h"

#undef WITHICONMOVE

#ifndef WITHICONMOVE
/*
class Y2CCIconDrag : public QIconDrag
{
    Q_OBJECT

public:
    Y2CCIconDrag( QWidget * dragSource, const char* name = 0 );
    
    const char* format( int i ) const;
    QByteArray encodedData( const char* mime ) const;
    void addFile(const QString&);
    private:
    QString filenames;
};
*/
#endif

/*
 *  needed for DnD
 */
class MyQIconView : public QIconView
{
	Q_OBJECT
	public:
	MyQIconView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
	~MyQIconView();
	protected:
	QDragObject * dragObject();
};

/*
 *  needed for DnD
 */
class MyQIconViewItem: public QIconViewItem, public YModData
{
	public:
	MyQIconViewItem ( QIconView * parent, const QString & text, const QPixmap & icon );
	~MyQIconViewItem();
};

#endif
