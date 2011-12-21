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
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QDir>
#include <QIcon>
#include <QHash>

#include "yqmodulegroupsmodel.h"

#define DEFAULT_GROUP_ICON	"yast-default-group.png"

bool paircmp( QPair<int, QString> p1, QPair<int, QString> p2)
{
	if ( p1.first != p2.first )
	   return p1.first < p2.first ;	
	else
	    return p1.second < p2.second;	
};

YQModuleGroupsModel::YQModuleGroupsModel( QObject * parent )
    : YQDesktopFilesModel(parent)
{

    QString default_dir( GROUPS_DESKTOP_DIR );
    QString alt_dir = QString( getenv( "YAST2_DESKTOP_DIR") );   

    addDesktopFilesPath( alt_dir.isEmpty() ? default_dir : alt_dir + "/groups/");
    addIconPath( ICON_DIR );
    addIconPath( FALLBACK_ICON_DIR );
    reindex();
    //addPersonalStuff();
}

YQModuleGroupsModel::~YQModuleGroupsModel()
{
}

QModelIndex YQModuleGroupsModel::indexForGroupId( const QString &groupid ) const
{
    return indexForValue( "X-SuSE-YaST-Group", groupid );
}

QString YQModuleGroupsModel::groupId( const QModelIndex &idx ) const
{
    QModelIndex grpid = index( idx.row(), Group);
    // retrieve the groupid
    QString groupid = data( grpid, Qt::UserRole ).toString();
    return groupid;
}

QVariant YQModuleGroupsModel::valueFromFile ( QString &fname, int key )
{
   int pos = d->desktop_files.indexOf( fname );

   if ( pos == -1 )
	return QVariant();

   QModelIndex idx = index( pos, key );
   return data( idx, Qt::UserRole);

}         
 
void YQModuleGroupsModel::addPersonalStuff()
{
    QList < QPair <QString, QString> > fake; 

    fake << QPair<QString, QString> ( "Personal.desktop", "Personal") 
	 << QPair<QString, QString> ( "Favourites.desktop", "Favourites") 
	 << QPair<QString, QString> ( "Recent.desktop", "Recently Used")
	 << QPair<QString, QString> ( "Modules.desktop", "Modules");

    QPair <QString, QString> p; 

    foreach( p, fake )
    {
	addDesktopFile( p.first );
	addPropertyToDesktopFile( p.first, "Name", p.second );
	addPropertyToDesktopFile( p.first, "X-SuSE-YaST-Group", p.second );
    }

    typeMap["Personal"] = Headline; 
    typeMap["Modules"] = Headline; 
    typeMap["Favourites"] = Special; 
    typeMap["Recently Used"] = Special; 

}

void YQModuleGroupsModel::reindex()
{
    // pairs, because sort keys don't have to be unique :(
    QList< QPair< int, QString> > sortKeys;
    QMap < QString, QString > groupNames; 

    QStringListIterator it( d->desktop_files );  

    while (it.hasNext())
    {
        QString dfile(it.next());
	int sortKey = valueFromFile( dfile, YQDesktopFilesModel::SortKey).toInt();
	QString name = valueFromFile( dfile, YQDesktopFilesModel::Name).toString();
        sortKeys.append( qMakePair( sortKey, name) ); 
	groupNames.insert( name, dfile );
    }

   qSort(sortKeys.begin(), sortKeys.end(), paircmp);

   d->desktop_files.clear();
   QPair<int,QString> p; 
   foreach( p, sortKeys )
   {
	d->desktop_files << groupNames[p.second];
   } 
    
}

QVariant YQModuleGroupsModel::data( const QModelIndex &index, int role ) const
{
    if (role == Qt::FontRole )
    {
	QString name = propertyValue( index, "Name" ).toString();
	QFont f;

	if( typeMap[name] == Headline)
	{
	    f.setBold(true);
	    QFontMetrics fm(f);
	    f.setPixelSize( (int) (fm.height() * 1.05) );
	    QVariant myData =  QVariant(f);
	    return myData;
	}
	else
 	    return QVariant();
    }
    else if ( role == Qt::DecorationRole )
    {
	QVariant icon = YQDesktopFilesModel::data( index, role );
	QString fallback_icon = "yast-default-group";

	if ( icon.isValid() )
	    return icon;
	else
	{
	    return findIcon( fallback_icon );
	}
    }
    else
        return YQDesktopFilesModel::data(index, role);
}

#include "yqmodulegroupsmodel.moc"
