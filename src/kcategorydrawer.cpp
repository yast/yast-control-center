/**
  * This file is part of the KDE project
  * Copyright (C) 2007 Rafael Fernández López <ereslibre@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
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

#include "kcategorydrawer.h"

#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QDebug>

#define HORIZONTAL_HINT 3

KCategoryDrawer::KCategoryDrawer()
{
}

KCategoryDrawer::~KCategoryDrawer()
{
}

void KCategoryDrawer::drawCategory(const QModelIndex &index,
                                   int /*sortRole*/,
                                   const QStyleOption &option,
                                   QPainter *painter) const
{
    const QString category = index.model()->data(index, KCategorizedSortFilterProxyModel::CategoryDisplayRole).toString();

    QColor color;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    
    color = option.palette.color(QPalette::WindowText);
    QColor bgcolor = option.palette.color(QPalette::Base);
    

    QStyleOptionViewItemV4 viewOptions;
    viewOptions.rect = option.rect;
    viewOptions.palette = option.palette;
    viewOptions.direction = option.direction;
    viewOptions.viewItemPosition = QStyleOptionViewItemV4::OnlyOne;
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &viewOptions, painter, 0);

    QFont painterFont = painter->font();
    painterFont.setWeight(QFont::Bold);
    QFontMetrics metrics(painterFont);
    painter->setFont(painterFont);


    QRect lineRect(option.rect.left() + 10,
                   option.rect.bottom() - 16 - option.fontMetrics.height(),
                   option.rect.width() - 20,
                   1);

    painter->fillRect(lineRect,  option.palette.brush(QPalette::Highlight) );

    painter->setPen(color);

    QRect textRect(option.rect);
    textRect.setLeft(textRect.left() + HORIZONTAL_HINT);
    textRect.setRight(textRect.right() - HORIZONTAL_HINT);
    painter->fillRect(option.rect, bgcolor );
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft,
    metrics.elidedText(category, Qt::ElideRight, option.rect.width()));

    painter->restore();
}

int KCategoryDrawer::categoryHeight(const QModelIndex &index, const QStyleOption &option) const
{
    Q_UNUSED(index);

    return option.fontMetrics.height() + 16 /* 3 separator; 1 gradient */;
}
