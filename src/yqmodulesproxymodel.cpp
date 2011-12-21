 /****************************************************************************
|
| Copyright (c) 2011 Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
 \***************************************************************************/



#include "yqmodulesproxymodel.h"

YQModulesProxyModel::YQModulesProxyModel( QObject * parent )
    : KCategorizedSortFilterProxyModel(parent)
{
}

YQModulesProxyModel::~YQModulesProxyModel()
{
}

bool YQModulesProxyModel::subSortLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return KCategorizedSortFilterProxyModel::subSortLessThan(left, right);
}

int YQModulesProxyModel::compareCategories(const QModelIndex &left, const QModelIndex &right) const
{
    return KCategorizedSortFilterProxyModel::compareCategories(left, right);
}



// QVariant YQModulesProxyModel::data( const QModelIndex &index, int role ) const
// {
//    if ( role == CategoryDisplayRole )
//    {
//    }
   
//    return YQDesktopFilesModel::data(index, role);
// }

#include "yqmodulesproxymodel.moc"
