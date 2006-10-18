/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   YaST2 Control Center			       |
|						     (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		yastmodules.cpp

  Authors:	Ludwig Nussel     <lnussel@suse.de>
		Stefan Hundhammer <sh@suse.de>

  Maintainer:	Stefan Hundhammer <sh@suse.de>


  Textdomain "control-center"
/-*/


#include "yastmodules.h"
#include "QY2Settings.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <iostream>

#include <qapplication.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qregexp.h>

using std::cout;
using std::cerr;
using std::endl;

#include "myintl.h"
#include "y2cc_globals.h"

#define DEBUG_MODE 		0
#define VERBOSE_GETTEXT		0
#define DESKTOP_TRANSLATIONS	"desktop_translations"


YModules::YModules()
{
    modList.setAutoDelete( true );	// delete items when they are removed
    groupList.setAutoDelete( true );	// delete items when they are removed

    bindtextdomain( DESKTOP_TRANSLATIONS, DESKTOP_TRANSLATIONS_DIR );
    bind_textdomain_codeset( DESKTOP_TRANSLATIONS, "utf8" );
}


YModules::~YModules()
{
}


bool YModules::init()
{
    initLang();

    if ( initGroups() && initModules() )
    {
	groupList.sort();
	modList.sort();
	emit modulesReady();
#if 0
	dumpModules();
	dumpGroups();
#endif

	return true;
    }
    else // Error
    {

	error  = _("No list of available sections could be created.\n\n"
		   "This means that YaST2 or this Control Center is\n"
		   "not correctly installed or that you do not have sufficient\n"
		   "permissions.\n\n");
	error += _("If the former describes your situation, re-install all YaST2 components;\n"
		   "if the latter is the case,  try to run the Control Center as root. \n"
		   "Also make sure that you do not run out of disk space. ");

	cerr << error << endl;

	return false;
    }
}


void YModules::initLang()
{
    const char *lang_cstr = getenv( "LANG" );

    if ( lang_cstr )
    {
	langWithCountry = lang_cstr;
	langWithCountry.replace( QRegExp( "[@\\.].*$" ), "" );	// remove .utf8 / @euro etc.

	lang = langWithCountry;
	lang.replace( QRegExp( "_.*$" ), "" );			// remove _DE etc.
    }
}


bool YModules::initGroups()
{
    QDir dir( GROUPS_DESKTOP_DIR "/", "*.desktop" );

    if ( ! dir.exists() )
    {
	emit modError( QString( _("Directory %1 does not exist.") ).arg( dir.absPath() ));
	return false;
    }

    QStringList desktop_files = dir.entryList();

    for ( QStringList::iterator it = desktop_files.begin();
	  it != desktop_files.end();
	  ++it )
    {
	readGroupDesktopFile( dir.path() + "/" + *it );
    }

    return true;
}


bool YModules::initModules()
{
    QDir dir( MODULES_DESKTOP_DIR "/", "*.desktop" );

    if ( ! dir.exists() )
    {
	emit modError( QString( _("Directory %1 does not exist.") ).arg( dir.absPath()));
	return false;
    }

    QStringList desktop_files = dir.entryList();

    for ( QStringList::iterator it = desktop_files.begin();
	  it != desktop_files.end();
	  ++it )
    {
	readModuleDesktopFile( dir.path(), *it );
    }

    return true;
}


bool YModules::readModuleDesktopFile( const QString & path, const QString & filename )
{
    QY2Settings desktopFile( path + "/" + filename );

    if ( desktopFile.readError() )
	return false;

    desktopFile.selectSection( "Desktop Entry" );

    if ( desktopFile[ "Hidden" ] == "true" )
	return true;

    YMod * mod = new YMod();
    CHECK_PTR( mod );


    //
    // Name
    //

    QString name = desktopFile[ QString( "Name[%1]" ).arg( langWithCountry ) ];

    if ( name.isEmpty() )
	name = desktopFile[ QString( "Name[%1]" ).arg( lang ) ];

    if ( name.isEmpty() )
    {
	QString msgid = QString( "Name(%1)" ).arg( filename );
	msgid += ": ";
	msgid += desktopFile[ "Name" ];
	name = QString::fromUtf8( dgettext( DESKTOP_TRANSLATIONS, msgid.ascii() ) );

	if ( name == msgid )	// no translation?
	    name = "";

#if VERBOSE_GETTEXT
	fprintf( stderr, "Reading key %s -> %s\n", msgid.ascii(), name.ascii() );
#endif
    }

    if ( name.isEmpty() )
	name = desktopFile[ "Name" ];

    mod->setName( name );


    //
    // GenericName
    //

    QString description = desktopFile[ QString( "GenericName[%1]" ).arg( langWithCountry ) ];

    if ( description.isEmpty() )
	description = desktopFile[ QString( "GenericName[%1]" ).arg( lang ) ];

    if ( description.isEmpty() )
    {
	QString msgid = QString( "GenericName(%1)" ).arg( filename );
	msgid += ": ";
	msgid += desktopFile[ "GenericName" ];
	description = QString::fromUtf8( dgettext( DESKTOP_TRANSLATIONS, msgid.ascii() ) );

	if ( description == msgid )	// no translation?
	    description = "";

#if VERBOSE_GETTEXT
	fprintf( stderr, "Reading key %s -> %s\n", msgid.ascii(), description.ascii() );
#endif
    }

    if ( description.isEmpty() )
	description = desktopFile[ "GenericName" ];

    mod->setDescription( description );

    QString icon = desktopFile[ "Icon" ];
    QRegExp extension( "\\.(png|jpg)$", false );	// case insensitive

    if ( icon.find( extension ) < 0 )	// no .png or .jpg extension?
	icon += ".png";			// assume .png

    mod->setIcon( icon );

    mod->setYCPName	( desktopFile[ "X-SuSE-YaST-Call"	] );
    mod->setGroup	( desktopFile[ "X-SuSE-YaST-Group"	] );
    mod->setArguments	( desktopFile[ "X-SuSE-YaST-Argument"	] );
    mod->setSortKey	( desktopFile[ "X-SuSE-YaST-SortKey"	] );
    mod->setGeometry	( desktopFile[ "X-SuSE-YaST-Geometry"	] );

    QString rootOnly =    desktopFile[ "X-SuSE-YaST-RootOnly"	];
    QRegExp yes( "^(true|yes|1)$", false );	// case insensitive

    mod->setRootFlag( rootOnly.find( yes ) >= 0 );

#if 0
    printf( "new Module: %s\n",	 (const char *) mod->getName() );
    printf( "\tgroup: %s\n",	 (const char *) mod->getGroup() );
    printf( "\ticon: %s\n",	 (const char *) mod->getIcon() );
    printf( "\tsortKey: %s\n\n", (const char *) mod->getSortKey() );
#endif

    addModule( mod );

    return true;
}


bool YModules::readGroupDesktopFile( const QString & filename )
{
    QY2Settings desktopFile( filename );

    if ( desktopFile.readError() )
	return false;

    desktopFile.selectSection( "Desktop Entry" );

    if ( desktopFile[ "Hidden" ] == "true" )
	return true;

    QString rawName = desktopFile[ "X-SuSE-YaST-Group"   ];
    QString sortKey = desktopFile[ "X-SuSE-YaST-SortKey" ];

    QString name = desktopFile[ QString( "Name[%1]" ).arg( langWithCountry ) ];

    if ( name.isEmpty() )
	name = desktopFile[ QString( "Name[%1]" ).arg( lang ) ];

    if ( name.isEmpty() )
	name = desktopFile[ "Name" ];

    QString icon = desktopFile.get( "Icon", "defaultgroup.png" );
    QRegExp extension( "\\.(png|jpg)$", false );	// case insensitive

    if ( icon.find( extension ) < 0 )	// no .png or .jpg extension?
	icon += ".png";			// assume .png

    ModGroup * grp = new ModGroup( rawName );

    groupList.first(); // move groupList.current() to the first group

    if ( groupList.find( grp ) >= 0 )
    {
	// group already existing
	qWarning( "Warning: Duplicate group " + rawName + " in " + filename );
    }
    else
    {
	grp->setName( name );
	grp->setIcon( icon );
	grp->setSortKey( sortKey );

	groupList.append( grp );
    }

#if 0
    printf( "new Group: %s\n",	 (const char *) grp->getRawName() );
    printf( "\tname: %s\n",	 (const char *) grp->getName() );
    printf( "\ticon: %s\n",	 (const char *) grp->getIcon() );
    printf( "\tsortKey: %s\n\n", (const char *) grp->getSortKey() );
#endif

    return true;
}


void YModules::addModule( YMod * module )
{
    if ( config.isroot || ! module->getRootFlag()
#if DEBUG_MODE
#warning FIXME - DEBUG MODE - displaying all modules for non-root users!
	 || true )
#else
	)
#endif
    {
	modList.append( module );

	ModGroup * tmpGrp = new ModGroup( module->getGroup() );

	groupList.first(); // move groupList.current() to the first group

	if ( groupList.find( tmpGrp ) < 0 )
	{
	    // group did not exist
	    qWarning( "Warning: new Group detected for Module " +
		      module->getName() + ", misspelled in .desktop file?");

	    tmpGrp->setIcon("defaultgroup.png");
	    tmpGrp->setSortKey( "zzzzz" );

	    tmpGrp = new ModGroup( module->getGroup() );
	    tmpGrp->addModule(module);
	    groupList.append( tmpGrp );
	}
	else
	{
	    // groupList.current() points now to what groupList.find() found

	    groupList.current()->addModule( module );

	    delete tmpGrp;
	    tmpGrp=0;
	}
    }
    else // user doesn't have sufficient permissions for this module
    {
	delete module;
    }
}

int YModules::numGroups()
{
    return groupList.count();
}

ModGroup* YModules::firstGroup()
{
    return groupList.first();
}

ModGroup* YModules::nextGroup()
{
    return groupList.next();
}

ModGroup* YModules::setGroup(uint idx)
{
    return groupList.at(idx);
}

ModGroup* YModules::getGroup() const
{
    return groupList.current();
}

void YModules::dumpModules()
{
    ModuleListIterator it( modList );
    printf( "*** Available Modules ***\n\n" );

    while ( *it )
    {
	// Beware: cout << (const char *) 0 doesn't work (overwrites memory),
	// and QString::null returns  such a (const char *) 0 !

	printf( "Module: %s\n",	(const char *) (*it)->getName()    );
	printf( "\tgroup: %s\n",	(const char *) (*it)->getGroup()   );
	printf( "\ticon: %s\n",	 	(const char *) (*it)->getIcon()    );
	printf( "\tsortKey: %s\n\n", 	(const char *) (*it)->getSortKey() );

	++it;
    }
}

void YModules::dumpGroups()
{
    GroupListIterator it( groupList );
    printf( "*** Available Groups ***\n\n" );

    while ( *it )
    {
	// Beware: cout << (const char *) 0 doesn't work (overwrites memory),
	// and QString::null returns  such a (const char *) 0 !

	printf( "Group:\t%s - ",	(const char *) (*it)->getRawName() );
	printf( "%s\n",			(const char *) (*it)->getName()    );
	printf( "\ticon: %s\n",	 	(const char *) (*it)->getIcon()    );
	printf( "\tsortKey: %s\n\n", 	(const char *) (*it)->getSortKey() );

	++it;
    }
}


void YModules::runModule( const YMod* module)
{
    if (!module)
    {
	cerr << "got NULL pointer in " << __FILE__ << " line " << __LINE__ << endl;
	return;
    }

    QString geo = module->getGeometry();
    QString ycpname = module->getYCPName();
    QString args = module->getArguments();

    QString cmd;

    // let the shell do the work
    if (!geo.isEmpty())
    {
	cmd += "Y2_GEOMETRY=\"" + geo + "\" ";
    }
    cmd += "/sbin/yast2 ";
    cmd += ycpname;

    if ( config.fullscreen )	cmd += " --fullscreen ";
    if ( config.noBorder   )	cmd += " --noborder ";

    if (!args.isEmpty())
    {
	QString arguments = args;
	arguments.replace (QRegExp (" "), "' '" );
	cmd += " '";
	cmd += arguments;
	cmd += "'";
    }
    cmd += " &";

    qDebug( QString("Command: ") + cmd);

    system(cmd);
}


const QString* YModules::getErrorString() const
{
    return &error;
}

/**********************/

YMod::YMod ()
{
    // selected yast2 module does not have a name
    name = _("Unnamed");
    requiresroot = true;
    iconfilename = "default.png";
    group = "Misc";
    textdomain = "control-center";
    // selected yast2 module does not have a short description for statusbar
    description = _("no description available");
}

YMod::YMod(const YMod&)
{
    cerr << "Copy constructor" << endl;
    exit(1);
}

YMod::YMod(const QString& Name,const QString& YCPName,const QString& Group,const QString& Description,const QString& IconFileName)
{
    name=Name;
    ycpname=YCPName;
    description=Description;
    iconfilename=IconFileName;
    group=Group;
    requiresroot=true;
}

YMod::~YMod()
{
}

void YMod::setName(const QString& Name)
{
    name=Name;
}

void YMod::setYCPName(const QString& YCPName)
{
    ycpname=YCPName;
}

void YMod::setArguments(const QString& args)
{
    arguments=args;
}

void YMod::setGeometry(const QString& geo)
{
    geometry=geo;
}

void YMod::setSortKey(const QString& key)
{
    sortkey=key;
}

void YMod::setTextdomain(const QString& key)
{
    textdomain=key;
}

void YMod::setGroup(const QString& Group)
{
    group=Group;
}

void YMod::setDescription(const QString& Description)
{
    description=Description;
}

void YMod::setIcon(const QString& IconFileName)
{
    iconfilename=IconFileName;
}

void YMod::setRootFlag(bool yes)
{
    requiresroot=yes;
}

QString YMod::getName() const
{
    return name;
}

QString YMod::getYCPName() const
{
    return ycpname;
}

QString YMod::getArguments() const
{
    return arguments;
}

QString YMod::getGeometry() const
{
    return geometry;
}

QString YMod::getSortKey() const
{
    return sortkey;
}

QString YMod::getTextdomain() const
{
    return textdomain;
}

QString YMod::getGroup() const
{
    return group;
}

QString YMod::getDescription() const
{
    return description;
}

QString YMod::getIcon() const
{
    return iconfilename;
}

bool YMod::getRootFlag() const
{
    return requiresroot;
}


bool YMod::operator<(const YMod& b)
{
    bool value; //=(group<b.group);

    if (sortkey.isEmpty() && b.sortkey.isEmpty())
    {
	value=(name<b.name);
    }
    else if (sortkey.isEmpty() && !b.sortkey.isEmpty())
    {
	value=(name<b.sortkey);
    }
    else if (!sortkey.isEmpty() && b.sortkey.isEmpty())
    {
	value=(sortkey<b.name);
    }
    else
    {
	if (sortkey!=b.sortkey)
	{
	    value=(sortkey<b.sortkey);
	}
	else
	{
	    value=(name<b.name);
	}
    }

    return value;
}


bool YMod::operator==(const YMod& b)
{
    bool value;//=(group==b.group);

    if (sortkey.isEmpty() && b.sortkey.isEmpty()) value=(name==b.name);
    else if (sortkey.isEmpty() && !b.sortkey.isEmpty()) value=(name==b.sortkey);
    else if (!sortkey.isEmpty() && b.sortkey.isEmpty()) value=(sortkey==b.name);
    else
    {
	if (sortkey!=b.sortkey)
	    value=(sortkey==b.sortkey);
	else
	    value=(name==b.name);
    }

    return value;
}


/*
 *   class ModGroup
 */

ModGroup::ModGroup()
{
    //class YModules holds the primary pointer to the Data
    modules.setAutoDelete(false);
    sorted=true;
    icon="defaultgroup.png";
    sortkey="";
    name="";
    rawname="";
}

ModGroup::ModGroup(const QString& Name)
{
    modules.setAutoDelete(false);
    rawname=Name;
    icon="defaultgroup.png";
    sorted=true;
}

QString ModGroup::getName() const
{
    if (this->name.isEmpty()) return this->rawname;
    return this->name;
}

QString ModGroup::getRawName() const
{
    return this->rawname;
}

QString ModGroup::getIcon() const
{
    return this->icon;
}

QString ModGroup::getSortKey() const
{
    return this->sortkey;
}

void ModGroup::setName(const QString& Name)
{
    name=Name;
}

void ModGroup::setRawName(const QString& Name)
{
    rawname=Name;
}

void ModGroup::setSortKey(const QString& Key)
{
    sortkey=Key;
}

void ModGroup::setIcon(const QString& Icon)
{
    icon=Icon;
}

void ModGroup::addModule(YMod* Module)
{
    modules.append(Module);
    sorted=false;
}

YMod* ModGroup::first()
{
    if (sorted==false) {modules.sort(); sorted=true;}
    return modules.first();
}

YMod* ModGroup::last()
{
    if (sorted==false) {modules.sort(); sorted=true;}
    return modules.last();
}

YMod* ModGroup::prev()
{
    if (sorted==false) {modules.sort(); sorted=true;}
    return modules.prev();
}

YMod* ModGroup::next()
{
    if (sorted==false) {modules.sort(); sorted=true;}
    return modules.next();
}

YMod* ModGroup::current()
{
    if (sorted==false) {modules.sort(); sorted=true;}
    return modules.current();
}

bool ModGroup::isEmpty() const
{
    return modules.isEmpty();
}

ModGroup::~ModGroup()
{
}


// the icon does not matter here
bool ModGroup::operator<(const ModGroup& b)
{
    QString A,B;

    if (sortkey.isEmpty())
    {
	if (name.isEmpty())
	    A=rawname;
	else
	    A=name;
    }
    else
	A=sortkey;

    if (b.sortkey.isEmpty())
    {
	if (b.name.isEmpty())
	    B=b.rawname;
	else
	    B=b.name;
    }
    else
	B=b.sortkey;

    if ( A==B && A==sortkey && B==b.sortkey )
    {
	if (name.isEmpty() || b.name.isEmpty())
	{
	    A=rawname;
	    B=rawname;
	}
	else
	{
	    B=b.name;
	    A=name;
	}
    }

    return (A<B);
}


bool ModGroup::operator>( const ModGroup& b )
{
    QString A,B;

    if (sortkey.isEmpty())
    {
	if (name.isEmpty())
	    A=rawname;
	else
	    A=name;
    }
    else
	A=sortkey;

    if (b.sortkey.isEmpty())
    {
	if (b.name.isEmpty())
	    B=b.rawname;
	else
	    B=b.name;
    }
    else
	B=b.sortkey;

    if ( A==B && A==sortkey && B==b.sortkey )
    {
	if (name.isEmpty() || b.name.isEmpty())
	{
	    A=rawname;
	    B=rawname;
	}
	else
	{
	    B=b.name;
	    A=name;
	}
    }

    return (A>B);
}


bool ModGroup::operator==(const ModGroup& b)
{
    bool value;
    if (name.isEmpty() || b.name.isEmpty())
    {
	value=(rawname==b.rawname);
    }
    else
    {
	value=(name==b.name);
    }

    return value;
}



/*
 *  class YModData
 */

YModData::YModData()
{
    module=0L;
}

YModData::~YModData()
{
}

void YModData::setModule(const YMod* m)
{
    module=m;
}

const YMod* YModData::getModule() const
{
    return module;
}

#include <yastmodules.moc.cpp>
