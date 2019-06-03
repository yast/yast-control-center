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


#ifndef YQMODULESMODEL_H
#define YQMODULESMODEL_H

#include <QStringList>
#include "yqdesktopfilesmodel.h"

class YQModuleGroupsModel;

/**
 * this class represents YaST module groups
 * so they can be used in any GUI item
 *
 * @author Duncan Mac-Vicar P. <dmacvicar@suse.de>
 */
class YQModulesModel : public YQDesktopFilesModel
{
Q_OBJECT
public:

  enum UserRoleData
  {
    CommentRole	= 0x1BA81D0E
  };

  YQModulesModel( QObject * parent = 0 );
  ~YQModulesModel();

  virtual PropertyMap readDesktopFile( const QString &path ); 

  /**
   * reimplemented from YQDesktopFilesModel
   */
  virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

  /**
   * reimplement flags to make items non selectable and enabled
   * depending if they are installed
   */
  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

  /**
   * group index for a given model
   */
  QModelIndex groupForModule( const QModelIndex &index) const;

  /**
   * returns the group name for item with index
   */
  //QString groupNameForIndex( const QModelIndex &index ) const;

  QModelIndex firstModuleInGroup( const QModelIndex &index ) const; 

  /**
   * the groups model this modules model is using
   */
  YQModuleGroupsModel * groupsModel() const;

  bool isRoot();   

private:
  class Private;
  Private *const priv;

  void removeEmptyGroups();
};

#endif
