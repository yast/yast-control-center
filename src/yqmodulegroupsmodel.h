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


#ifndef YQMODULEGROUPSMODEL_H
#define YQMODULEGROUPSMODEL_H

#include <QStringList>
#include "yqdesktopfilesmodel.h"

/**
 * this class represents YaST module groups
 * so they can be used in any GUI item
 *
 * @author Duncan Mac-Vicar P. <dmacvicar@suse.de>
 */

class YQModuleGroupsModel : public YQDesktopFilesModel
{
Q_OBJECT
public:

  enum Type
  {
    Regular,
    Headline,
    Special
  };

  YQModuleGroupsModel( QObject * parent = 0 );
  ~YQModuleGroupsModel();

  virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

  /**
   * Returns the model index from an id (like "Virtualization")
   * 
   * Useful if you need to get data, and you don't know the index but only
   * the group id, which you may had obtain in the modules model for example.
   */
  QModelIndex indexForGroupId( const QString &groupid ) const;

  QVariant valueFromFile( QString &fname, int key );

  /**
   * Returns the group id for a given group
   */
  QString groupId( const QModelIndex &idx ) const;

private:
    QMap <QString, Type> typeMap;    

    void reindex();
    void addPersonalStuff();
};

#endif
