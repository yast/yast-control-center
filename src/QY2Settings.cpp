/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt classes		       |
|						     (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:	      QY2Settings.cpp

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt class - it can be used independently of YaST2.

/-*/

// -*- c++ -*-


#include "QY2Settings.h"

#include <qtextstream.h>
#include <qregexp.h>


QY2Settings::QY2Settings( const QString & fileName, AccessMode accessMode )
    : _fileName( fileName )
    , _accessMode( accessMode )
{
    _sections.setAutoDelete( true );
    initSections();
    
    if ( _accessMode == ReadOnly || _accessMode == ReadWrite )
	read();
}

void QY2Settings::initSections()
{
    _defaultSection = new Section( QString::null );
    CHECK_PTR( _defaultSection );

    _currentSection = _defaultSection;
    _sections.insert( QString::null, _defaultSection );
}



QY2Settings::~QY2Settings()
{
    if ( _dirty && _accessMode != ReadOnly )
	write();
}


bool QY2Settings::selectSection( const QString & sectionName )
{
    _currentSection = _sections[ sectionName ];

    if ( _currentSection )
    {
	return true;
    }
    else
    {
	_currentSection = _defaultSection;
	return false;
    }
}


void QY2Settings::selectDefaultSection()
{
    _currentSection = _defaultSection;
}


QStringList QY2Settings::sections( bool includeUnnamed ) const
{
    QStringList sectionList;
    SectionIterator it( _sections );

    while ( *it )
    {
	QString sectionName = (*it)->name();

	if ( includeUnnamed || ! sectionName.isEmpty() )
	    sectionList.append( sectionName );
	
	++it;
    }

    return sectionList;
}


QString QY2Settings::get( const QString & key, const QString & fallback ) const
{
    // Can't use operator[] here since we have a pointer
    
    Section::const_iterator it = _currentSection->find( key );

    if ( it == _currentSection->constEnd() )
	return fallback;

    return it.data();
}


QString QY2Settings::operator[] ( const QString & key )
{
    return get( key );
}


bool QY2Settings::hasKey( const QString & key )
{
    return _currentSection->contains( key );
}


QStringList QY2Settings::keys() const
{
    QStringList keyList;
    
    for ( Section::const_iterator it = _currentSection->constBegin();
	  it != _currentSection->constEnd();
	  ++it )
    {
	keyList.append( it.key() );
    }

    return keyList;
}


void QY2Settings::read()
{
    if ( _accessMode == WriteOnly )
	return;

    initSections();
    QTextIStream str( &_fileName );

    while ( ! str.atEnd() )
    {
	str.skipWhiteSpace();
	QString line = str.readLine();

	
	// Skip comment lines
	
	if ( line.startsWith( "#" ) )
	    continue;

	if ( line.startsWith( "[" ) )
	{
	    // New section

	    line.replace( QRegExp( "^\\[\\s*" ), "" );		
	    line.replace( QRegExp( "\\s*\\].*$" ), "" );
	    addSection( line );
	}
	else if ( line.contains( "=" ) )
	{
	    QString key   = line.section( "=", 0 );
	    QString value = line.section( "=", 1 );

	    key.stripWhiteSpace();
	    
	    value.stripWhiteSpace();
	    value.replace( QRegExp( "^\"" ), "" );	// strip leading "
	    value.replace( QRegExp( "\"$" ), "" );	// strip trailing "
	    value.replace( "\\\"", "\"" );		// un-escape "

	    set( key, value );
	}
	else
	{
#warning TODO: Handle syntax error
	}
    }

    _dirty = false;
}


void QY2Settings::write()
{
    if ( _accessMode == ReadOnly )
	return;

    QTextOStream str( &_fileName );
    str.setEncoding( QTextStream::UnicodeUTF8 );

    SectionIterator sectIt( _sections );

    while ( *sectIt )
    {
	Section * sect = *sectIt;

	// Section header
	
	if ( ! sect->name().isEmpty() )
	    str << "[" << sect->name() << "]" << endl;

	// value=key pairs for this section
	
	for ( Section::iterator it = sect->begin();
	      it != sect->end();
	      ++it )
	{
	    QString value = it.data();
	    value.replace( "\"", "\\\"" );	// Escape embedded " with \"
	    
	    str << it.key() << "= \"" << value << "\"" << endl;
	}

	str << endl;
	++sectIt;
    }
	
    _dirty = false;
}


void QY2Settings::set( const QString & key, const QString & value )
{
    _currentSection->insert( key, value );
    _dirty = true;
}


void QY2Settings::addSection( const QString & sectionName )
{
    _currentSection = _sections[ sectionName ]; // already there?

    if ( _currentSection )			// -> use it
	return;

    _currentSection = new Section( sectionName );
    CHECK_PTR( _currentSection );

    _sections.insert( sectionName, _currentSection );
    _dirty = true;
}


void QY2Settings::clearSection()
{
    _currentSection->clear();
    _dirty = true;
}


