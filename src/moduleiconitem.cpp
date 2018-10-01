/**
 * This file is part of the System Settings package
 * Copyright (C) 2005 Benjamin C Meyer
*                    <ben+systempreferences at meyerhome dot net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "moduleiconitem.h"
#include "kcmoduleinfo.h"

#include <climits>

#include <qapplication.h>
#include <qpainter.h>
#include <iostream>

#include <QDebug>


#define IMAGE_SIZE 32
#define ICON_WIDTH 100

ModuleIconItemDelegate::ModuleIconItemDelegate(QObject *parent, const QStyle *style) : 
    QItemDelegate(parent)
    , lviewstyle( style )
{
}

void ModuleIconItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	
	QStyle *style;
	bool selected = option.state & QStyle::State_Selected || option.state & QStyle::State_HasFocus;
	bool mouseover = option.state & QStyle::State_MouseOver;
	const QSize &decorationSize = option.decorationSize;

	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option)) {
		style = v3->widget->style();
		if (!v3->widget->hasFocus()) selected = false;
	} else {
		style = QApplication::style();
	}

	QFont f = painter->font();
	if (selected || mouseover)
	{
	    if (selected) 
	    {
		painter->fillPath(roundedRectangle(option.rect, 0), option.palette.brush(QPalette::Highlight));
		painter->setPen(option.palette.color(QPalette::HighlightedText));
	    }
	    else
	    {
		QColor hover = option.palette.color(QPalette::Highlight);
		hover.setAlpha(75);
		painter->fillPath(roundedRectangle(option.rect, 0), hover);
		painter->setPen( option.palette.color( QPalette::WindowText ) );
	    }
	    QFont newFont = option.font;
	    newFont.setPointSize( option.font.pointSize() - 2);
	    QFontMetrics newFm = QFontMetrics( newFont );

	    QString descr = newFm.elidedText( index.data( YQModulesModel::GenericNameRole ).toString(), Qt::ElideRight, option.rect.width() );
	    QRect tr = option.rect;
	    tr.setTop( tr.top() + decorationSize.height() + 2*style->pixelMetric( QStyle::PM_FocusFrameVMargin) );
	    tr.setLeft( tr.left() + 2*style->pixelMetric( QStyle::PM_FocusFrameHMargin ));

	    painter->setFont( newFont);
	    painter->drawText( tr, Qt::AlignLeft | Qt::TextSingleLine, descr ); 
	    
	}
	painter->setFont( f );

	QIcon::Mode iconMode = QIcon::Normal;
	if (selected) iconMode = QIcon::Selected;
	const QPixmap &pixmap = qvariant_cast<QIcon>(index.data(Qt::DecorationRole)).pixmap(option.decorationSize, iconMode);
	int iconX = option.rect.left() + style->pixelMetric( QStyle::PM_FocusFrameHMargin );
	int iconY = option.rect.top() + style->pixelMetric( QStyle::PM_FocusFrameVMargin );
	painter->drawPixmap(iconX, iconY, decorationSize.width(), decorationSize.height(), pixmap);
	
	QRect textRectangle = option.rect;
	textRectangle.setLeft (textRectangle.left() + decorationSize.width() + 2*style->pixelMetric(QStyle::PM_FocusFrameHMargin));
	textRectangle.setHeight( decorationSize.height() + 2*style->pixelMetric(QStyle::PM_FocusFrameVMargin));
	painter->drawText(textRectangle, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, index.data(Qt::DisplayRole).toString());
	painter->restore();
}

QSize ModuleIconItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
   QString text = index.data( Qt::DisplayRole).toString();
   QVariant v  = index.data(Qt::FontRole);
   QFont f = qvariant_cast<QFont>(v);
   QFontMetrics fm( f );

   QSize s = QItemDelegate::sizeHint( option, index );
   int w =  2*lviewstyle->pixelMetric( QStyle::PM_FocusFrameHMargin ) + 2*lviewstyle->pixelMetric( QStyle::PM_ListViewIconSize ) + 8 + fm.boundingRect( text ).width();	

   return QSize( w, s.height()); 
}

// Method taken from KFileItemDelegate. Check whether it has been moved to
// kdefx/kdrawutil.cpp as the comment says on Fredrik's code. If so, remove
// this code (duplication), and use the library one.
QPainterPath ModuleIconItemDelegate::roundedRectangle(const QRectF &rect, qreal radius) const
{
	QPainterPath path(QPointF(rect.left(), rect.top() + radius));
	path.quadTo(rect.left(), rect.top(), rect.left() + radius, rect.top());         // Top left corner
	path.lineTo(rect.right() - radius, rect.top());                                 // Top side
	path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + radius);       // Top right corner
	path.lineTo(rect.right(), rect.bottom() - radius);                              // Right side
	path.quadTo(rect.right(), rect.bottom(), rect.right() - radius, rect.bottom()); // Bottom right corner
	path.lineTo(rect.left() + radius, rect.bottom());                               // Bottom side
	path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - radius);   // Bottom left corner
	path.closeSubpath();

	return path;
}


