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


#include <QSettings>
#include <QDir>
#include <QIcon>
#include <QMap>
#include <libintl.h>

#include "yqdesktopfilesmodel.h"

#define DESKTOP_TRANSLATIONS "desktop_translations"
    
YQDesktopFilesModel::YQDesktopFilesModel( QObject * parent )
    : QAbstractItemModel(parent)
    , d(new Private)
{
    connect(&d->watcher, SIGNAL(directoryChanged(const QString &)),
            SLOT(slotDesktopFilesDirectoryChanged( const QString & ) ) );

    bindtextdomain( DESKTOP_TRANSLATIONS, DESKTOP_TRANSLATIONS_DIR );
    bind_textdomain_codeset( DESKTOP_TRANSLATIONS, "utf8" );

}

YQDesktopFilesModel::~YQDesktopFilesModel()
{
    delete d;
}

QModelIndex YQDesktopFilesModel::indexForValue( const QString &key, const QVariant &value ) const
{
    PropertyMapCacheIterator it(d->cache);
    //qDebug() << "cache has "<< cache.size() << " entries";

    while ( it.hasNext() )
    {
        it.next();      
        // filename is the key of the iterator
        //QString filename = it.key(); 
        PropertyMap data(it.value());
        if ( data.contains(key) && ( data.value(key) == value ) )
        {
            // we found a match, lets figure out the position of
            // the file in the list
            int pos = d->desktop_files.indexOf(it.key());
            if ( pos  != -1  )
                return index(pos, 0);
        }
    }

//    qWarning() << "No index for key: " << key << " value: " << value.toString();
    return QModelIndex();
}

void YQDesktopFilesModel::addDesktopFilesPath( const QString &dir )
{
    d->desktop_dirs << dir;
    d->watcher.addPath(dir);
    readDesktopFilesDirectories();
    readDesktopFiles();
}

void YQDesktopFilesModel::addIconPath( const QString &dir )
{
    d->icon_dirs << dir;
}

void YQDesktopFilesModel::slotDesktopFilesDirectoryChanged( const QString &path )
{
    //emit dataChanged();
    //reset();
}

/**
 * read a YaST group .desktop file
 */
PropertyMap YQDesktopFilesModel::readDesktopFile( const QString &filename )
{
    // desktop file data
    PropertyMap data;
    
    QSettings desktopFile( filename, QSettings::IniFormat );

    // check if parsing was correct
    if ( desktopFile.status() != QSettings::NoError )
    {
        qWarning() << "Error reading desktop file " << filename;
        return data;
    }
            
    desktopFile.beginGroup( "Desktop Entry" );

    QStringList keys(desktopFile.allKeys());
    QStringListIterator keyit(keys);
    while (keyit.hasNext())
    {
        QString key(keyit.next());
        data.insert(key, desktopFile.value(key));
    }
    
    qDebug() << filename << " read with " << data.size() << " keys";
    
    return data;
}

void YQDesktopFilesModel::readDesktopFiles()
{
    //local copy 
    QStringList desktop_files = d->desktop_files;
    d->cache.clear();

    QStringListIterator it(desktop_files);
    
    int count = 0;
    int success = 0;
    
    while ( it.hasNext() )
    {
        QString filename = it.next();
        // first lookup in cache
        PropertyMap data;

        data = readDesktopFile(filename);    
        if ( ! data.empty() )
        {
            d->cache.insert(filename, data);
            qDebug() << filename << " has index " << count;
            success++;
        }
	// readDesktopFiles returned empty map - skip those
	else
	{
	    qDebug() << "Skipping " << filename << ", read returned no reasonable data";
	    d->desktop_files.removeAll( filename );
	}
        count++;
    }
    qDebug() << success << " desktop files read";
}

void YQDesktopFilesModel::readDesktopFilesDirectories()
{
    //d->desktop_dirs.clear();
   d->desktop_files.clear();
   
   QStringListIterator dirs(d->desktop_dirs);
    
   while ( dirs.hasNext() )
   {
       // read group desktop files
       QDir dir( dirs.next() + "/", "*.desktop" );

       if ( ! dir.exists() )
       {
           qWarning() << "Directory " << dir.absolutePath() << " does not exist";
       }
       else
       {
           QStringListIterator desktop_files(dir.entryList());
           while ( desktop_files.hasNext() )
           {
               QString desktop_file = desktop_files.next();
               d->desktop_files << ( dir.absolutePath() + "/" + desktop_file );
           }
           
       }
       qDebug() << d->desktop_files.size() << " desktop files in total";
   }
}

int YQDesktopFilesModel::columnCount( const QModelIndex & parent ) const
{
    return 1;
}

int YQDesktopFilesModel::rowCount( const QModelIndex & parent ) const
{
    return d->desktop_files.size();
}

bool YQDesktopFilesModel::removeRow(int row )
{

   QString key = d->desktop_files.value( row );
   qDebug() << "removing empty " << key;
   d->desktop_files.removeAt( row );

   d->cache.remove( key );

   return true;
   
}

QModelIndex YQDesktopFilesModel::index( int row, int column, const QModelIndex & parent ) const
{
    return createIndex(row, column);
}

QVariant YQDesktopFilesModel::propertyValue( const QModelIndex &index, const QString &key ) const
{
    // first lookup in cache
    if ( index.row() > d->desktop_files.size() - 1 )
        return QVariant();
    
    // get file at position
    QString filename = d->desktop_files.at(index.row());
    
    PropertyMap data;
    if ( d->cache.contains(filename) )
    {
        data = d->cache.value(filename);
        if ( data.contains(key) )
            return data.value(key);
    }
    return QVariant();
}

QVariant YQDesktopFilesModel::translatedPropertyValue( const QModelIndex &index, const QString &key ) const
{
    QString fname =  d->desktop_files.value( index.row() );
    QFileInfo fi(fname); 
    QVariant value = propertyValue(index, key);
    QString valueAsString = "";

    // We can get QString, as well as QStringList if the .desktop key is csv
    // (Qt's being too smart here and converts csv automatically to QStringList
    // unless it's quoted). I'm not really happy about this solution, would be 
    // better to enforce quoting csv, but .desktop standard does not enforce 
    // quoting either (bnc#550085)
    switch( value.type())
    {
	case QVariant::String: {
	    valueAsString = value.toString();
	    break;
	} 
	case QVariant::StringList: {
	    valueAsString = value.toStringList().join(", ");
	    break;
	}
	default:
	    break;
    }


    QString msgid = QString( "%1(%2)" ).arg( key, fi.fileName() ); 
    msgid += ": ";
    msgid += valueAsString;

    QString msgstr = QString::fromUtf8( dgettext ( DESKTOP_TRANSLATIONS, msgid.toAscii()) );

    if( msgid == msgstr)
	return valueAsString;
    else
	return msgstr;

}
 
QVariant YQDesktopFilesModel::findIcon(  QString &icon ) const
{
    QRegExp extension( "\\.(png|jpg)$", Qt::CaseInsensitive );
    if ( icon.indexOf( extension ) < 0 )	// no .png or .jpg extension?
        icon += ".png";			// assume .png
    QStringListIterator it(d->icon_dirs);
    while (it.hasNext())
    {
        QString icondir(it.next());
        if ( QFile::exists(icondir + "/" + icon) )
        {
            return QIcon(icondir + "/" + icon);
        }
    }
    return QVariant();

}
 
void YQDesktopFilesModel::sort( int, Qt::SortOrder order )
{
    emit layoutAboutToBeChanged();
    if (order == Qt::AscendingOrder)
        qSort(d->desktop_files.begin(), d->desktop_files.end(), qLess<QString>());
    else
        qSort(d->desktop_files.begin(), d->desktop_files.end(), qGreater<QString>());
    emit layoutChanged();
    return;
}

QVariant YQDesktopFilesModel::data( const QModelIndex &index, int role ) const
{
    QStringListIterator it(d->desktop_files);

    // check the role and column
    if ( role == Qt::DisplayRole )
    {
	return translatedPropertyValue( index, "Name");
    }
    else if ( role == Qt::DecorationRole )
    {
        QString icon = propertyValue( index, "Icon" ).toString();
	return findIcon( icon );
    }
    else if ( role == Qt::UserRole )
    {

        // user role has app specific data
        switch ( index.column() )
        {
        case Name:
            return propertyValue( index, "Name" );
	case GenericName:
	    return propertyValue ( index, "GenericName" );
        case Group:
            return propertyValue( index, "X-SuSE-YaST-Group" );
        case Icon:
            return propertyValue( index, "Icon" );
        case SortKey:
            return propertyValue( index, "X-SuSE-YaST-SortKey" );
	case Call:
	    return propertyValue ( index, "X-SuSE-YaST-Call" );
	case Argument:
	    return propertyValue ( index, "X-SuSE-YaST-Argument" );
        default:
            return QVariant();
        }                
    }
    else
    {
        return QVariant();
    }      
    return QVariant();
}

QModelIndex YQDesktopFilesModel::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}

void YQDesktopFilesModel::addDesktopFile( QString &fname )
{
    d->desktop_files.push_front( fname );
}

void YQDesktopFilesModel::addDesktopFiles( QStringList &flist )
{
    QStringListIterator it( flist );
    while (it.hasNext())
    {
        QString one_string(it.next());
        addDesktopFile( one_string);
    }
}

void YQDesktopFilesModel::addPropertyToDesktopFile( QString &fname, const QString &key, QString &value)
{
    if (d->cache.find( fname ) != d->cache.end())
    {
	d->cache[ fname ].insert( key, value );
    }
    else
    {
	PropertyMap pm;
	pm[key] = value;

	d->cache[ fname ] = pm;
    }
}


#include "yqdesktopfilesmodel.moc"
