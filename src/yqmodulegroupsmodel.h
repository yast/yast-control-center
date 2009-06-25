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
