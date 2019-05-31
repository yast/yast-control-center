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

#include <QDebug>
#include <QSettings>

#include "yqmodulesmodel.h"
#include "yqmodulesproxymodel.h"

#include "yqmodulegroupsmodel.h"
#include "yccmoduleinfoprovider.h"

#include <unistd.h>
#include <sys/types.h>

class YQModulesModel::Private
{
public:
    Private()
        : groupsModel(new YQModuleGroupsModel)
    {
         userIsRoot = ( getuid() == 0 ); 
    }

    ~Private()
    {
        delete groupsModel;
    }

    YQModuleGroupsModel *groupsModel;
    QMap<QModelIndex, QModelIndex> groupCache;
    QMap<QString, QString> groupStringCache;
    YCCModuleInfoProvider infoProvider;

    bool userIsRoot; 

};

    

YQModulesModel::YQModulesModel( QObject * parent )
    : YQDesktopFilesModel(parent)
    , priv(new Private)
{
    QString default_dir( MODULES_DESKTOP_DIR );
    QString alt_dir = QString( getenv( "YAST2_DESKTOP_DIR") );   

    addDesktopFilesPath( alt_dir.isEmpty() ? default_dir : alt_dir );
    removeEmptyGroups();
}

YQModulesModel::~YQModulesModel()
{
}

YQModuleGroupsModel * YQModulesModel::groupsModel() const
{
    return priv->groupsModel;
}

PropertyMap YQModulesModel::readDesktopFile( const QString &path )
{
    PropertyMap data;
    const QString rootOnly = "X-SuSE-YaST-RootOnly";
    const QString group = "X-SuSE-YaST-Group";
    const QString name = "GenericName";

    QSettings desktopFile( path, QSettings::IniFormat );

    // check if parsing was correct
    if ( desktopFile.status() != QSettings::NoError )
    {
        qWarning() << "Error reading desktop file " << path;
        return data;
    }
            
    desktopFile.beginGroup( "Desktop Entry" );

    QStringList keys(desktopFile.allKeys());
    QStringListIterator keyit(keys);

    // X-SuSE-YaST-RootOnly = true 
    // do not show those to normal user
    if ( !priv->userIsRoot && keys.contains( rootOnly ) && 
	  desktopFile.value(rootOnly).toBool() )
	return data;

    QString oneGroup;
    QString oneName;
    while (keyit.hasNext())
    {
        QString key(keyit.next());
        data.insert(key, desktopFile.value(key));

	// cache <groupname : modname> pairs to quickly look up
	// what's the first module in the group
	oneGroup = desktopFile.value(group).toString();
	oneName = desktopFile.value(name).toString();

	if ( priv->groupStringCache.find(oneGroup) != priv->groupStringCache.end() )
	{
	    if ( oneName < priv->groupStringCache[oneGroup])
		priv->groupStringCache[oneGroup] = oneName;
	}
	else
	    priv->groupStringCache.insert(oneGroup, oneName);
    }
   
    qDebug() << path << " read with " << data.size() << " keys";
  
    return data;

}

QModelIndex YQModulesModel::firstModuleInGroup( const QModelIndex &index ) const
{
    // what is the name of that group?
    QString grpid = groupsModel()->groupId(index);

    if ( grpid.isEmpty() )
        return QModelIndex();
    
    qDebug() << "Look up 1st module in group " << grpid;

    QString firstMod = priv->groupStringCache[grpid];
    return indexForValue("GenericName", firstMod );
}

Qt::ItemFlags YQModulesModel::flags ( const QModelIndex & index ) const
{
    //qDebug() << "requesting flags for module";
    return Qt::ItemIsEnabled;
}

QModelIndex YQModulesModel::groupForModule( const QModelIndex &idx) const
{
    if ( priv->groupCache.contains(idx) )
        return priv->groupCache[idx];
    
    // create an index for the module group's string id
    QModelIndex modgrpid = index( idx.row(), Group);
    // retrieve the groupid
    QString groupid = data( modgrpid, Qt::UserRole ).toString();

    QModelIndex groupindex = groupsModel()->indexForGroupId( groupid );

    priv->groupCache[idx] = groupindex;
    
    return groupindex;
}

QVariant YQModulesModel::data( const QModelIndex &index, int role ) const
{
    // this 2 roles are required if the model is expected to work
    // with the categorized view
    if ( role == KCategorizedSortFilterProxyModel::CategoryDisplayRole )
    {
        return groupsModel()->data( groupForModule(index) );
    }
    else if ( role == KCategorizedSortFilterProxyModel::CategorySortRole )
    {
	QModelIndex grp_idx = groupForModule( index );
	QModelIndex idx = createIndex( grp_idx.row(), YQDesktopFilesModel::SortKey );
	
        return groupsModel()->data( idx, Qt::UserRole); 
    }
    else if ( role == KCategorizedSortFilterProxyModel::KeywordsRole )
    {
	return translatedPropertyValue( index, "X-SuSE-YaST-Keywords" );
    }
    else if ( role == CommentRole )
    {
        //QString tooltip = d->infoProvider.provideInfo( this, index );
        QString tooltip = translatedPropertyValue(index, "Comment").toString();
        return tooltip;       
    }
    else if ( role == Qt::DecorationRole )
    {
	QVariant icon = YQDesktopFilesModel::data( index, role );

	if ( icon.isValid() )
	    return icon;
	else
	{
	    return groupsModel()->data( groupForModule( index ), role );
	}	
    }
    else // otherwise just use the parent class
    {    
        return YQDesktopFilesModel::data(index, role);
    }
}

bool YQModulesModel::isRoot()
{
    return priv->userIsRoot;
}

void YQModulesModel::removeEmptyGroups()
{
    QStringList groups; 
    for (int i = 0; i < groupsModel()->rowCount( QModelIndex() ); i++)
    {
	QModelIndex idx = createIndex( i, 0 );
 	groups <<  groupsModel()->groupId( idx );
    }

    // filter out the .desktop files with unknown or missing group otherwise grouping aborts later
    PropertyMapCacheMutableIterator it(d->cache);
    while ( it.hasNext() )
    {
        it.next();

        if ( !it.value().contains("X-SuSE-YaST-Group") )
        {
            QString file = it.key();
            qWarning() << "Warning: Skipping file" << file << ": missing group attribute (X-SuSE-YaST-Group)";
            d->desktop_files.removeAll( file );
            it.remove();
            continue;
        }

        QString group = it.value().value("X-SuSE-YaST-Group").toString();
        if ( !groups.contains( group ) )
        {
            QString file = it.key();
            qWarning() << "Warning: Skipping file" << file << ": unknown group (X-SuSE-YaST-Group)" << group
                << "- currently defined groups:" << groups;
            d->desktop_files.removeAll( file );
            it.remove();
        }
    }

    QStringListIterator  git(groups);
    while (git.hasNext())
    {
	QModelIndex groupidx = groupsModel()->indexForGroupId( git.next() );
	QModelIndex modidx = firstModuleInGroup( groupidx );

	if (!modidx.isValid())
	{
	   groupsModel()->removeRow( groupidx.row() );
	   qDebug() << "Empty group " << groupsModel()->data( groupidx ).toString();
	}
    }

}
    
#include "yqmodulesmodel.moc"
