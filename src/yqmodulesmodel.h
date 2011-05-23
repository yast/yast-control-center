/*---------------------------------------------------------------------\
|                                                                      |
|                        __   __    _____ _____                        |
|                        \ \ / /   /  ___|_   _|                       |
|                         \ V /__ _\ `--.  | |                         |
|                          \ // _` |`--. \ | |                         |
|                          | | (_| /\__/ / | |                         |
|                          \_/\__,_\____/  \_/                         |
|                                                                      |
\---------------------------------------------------------------------*/

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
    GenericNameRole	= 0x1BA81D0E
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
