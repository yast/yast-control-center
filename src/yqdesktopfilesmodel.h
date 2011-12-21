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

#ifndef YQDESKTOPFILESMODEL_H
#define YQDESKTOPFILESMODEL_H

#include <QAbstractItemModel>
#include <QDebug>
#include <QStringList>
#include <QFileSystemWatcher>

/**
 * this class represents a directory with .desktop files
 * Each row is a desktop file, and each column, its
 * attributes.
 *
 * @author Duncan Mac-Vicar P. <dmacvicar@suse.de>
 */

typedef QMap<QString, QVariant> PropertyMap;
typedef QMapIterator<QString, QVariant> PropertyMapIterator;
// desktop file -> properties
typedef QMap<QString, PropertyMap > PropertyMapCache;
typedef QMapIterator<QString, PropertyMap> PropertyMapCacheIterator;

class cmp;

class YQDesktopFilesModel : public QAbstractItemModel
{
Q_OBJECT

public:
    enum Columns
    {
      Name,
      GenericName,
      Group,
      Icon,
      SortKey,
      ColumnCount,
      Call,
      Argument,
    };

    YQDesktopFilesModel( QObject * parent = 0 );
    ~YQDesktopFilesModel();

    /**
     * Add a directory where Desktop files will be searched
     */
    void addDesktopFilesPath( const QString & dir );
    
    /**
     * Add a directory where to look for icons
     */
    void addIconPath( const QString &dir );

    // reimplemented methods from QAbstractItemModel
    
    virtual int columnCount( const QModelIndex & parent ) const;
    virtual int rowCount( const QModelIndex & parent ) const;
    virtual bool removeRow ( int row );
  
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    /**
     * returns the index of the first item for which the key has
     * the provided value
     */
    QModelIndex indexForValue( const QString &key, const QVariant &value ) const;

    /**
     * returns the desktop file property for the given key
     */
    QVariant propertyValue( const QModelIndex &index, const QString &key ) const;
    QVariant translatedPropertyValue( const QModelIndex &index, const QString &key ) const;
    QVariant findIcon ( QString &icon ) const;

    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    virtual QModelIndex parent( const QModelIndex & index ) const;

    void addDesktopFile( QString &fname);
    void addDesktopFiles( QStringList &flist);
    void addPropertyToDesktopFile( QString &fname, const QString &key, QString &value );

protected:
    void readDesktopFilesDirectories();
    void readDesktopFiles();
    virtual PropertyMap readDesktopFile( const QString &path );

    class Private {
        public:

        Private() {}

        ~Private() { /*qWarning() << "Destroy!";*/ }
        
        // directories to read
        QStringList desktop_dirs;

        QStringList icon_dirs;

        // list of .desktop files read from disk
        QStringList desktop_files;

        // already read data
        // row -> ( key -> data )
        PropertyMapCache cache;
        
        QFileSystemWatcher watcher;
    };

    Private *const d;
   
protected slots:
    void slotDesktopFilesDirectoryChanged( const QString &path );

};

#endif
