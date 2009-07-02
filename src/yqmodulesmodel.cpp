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
    YCCModuleInfoProvider infoProvider;

    bool userIsRoot; 

};

    

YQModulesModel::YQModulesModel( QObject * parent )
    : YQDesktopFilesModel(parent)
    , priv(new Private)
{
    addDesktopFilesPath( MODULES_DESKTOP_DIR );
    addIconPath( ICON_DIR );
    addIconPath( FALLBACK_ICON_DIR );
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

    while (keyit.hasNext())
    {
        QString key(keyit.next());
        data.insert(key, desktopFile.value(key));
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
    
    qDebug() << "look element with value " << grpid;
    
    // Look first item that has property set to that group id.
    return indexForValue( "X-SuSE-YaST-Group", grpid );
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
    else if ( role == KCategorizedSortFilterProxyModel::GenericNameRole )
    {
	
        //QString tooltip = d->infoProvider.provideInfo( this, index );
        QString tooltip = translatedPropertyValue(index, "GenericName").toString();
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

void YQModulesModel::removeEmptyGroups()
{
    QStringList groups; 
    for (int i = 0; i < groupsModel()->rowCount( QModelIndex() ); i++)
    {
	QModelIndex idx = createIndex( i, 0 );
 	groups <<  groupsModel()->groupId( idx );
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
