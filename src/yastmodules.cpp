/***************************************************************************
            yastmodules.cpp  -  Interface to ycp, handling of modules
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    Copyright           : (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

/*
  Textdomain "control-center"
*/

#include <yastmodules.h>
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

using std::cout;
using std::cerr;
using std::endl;

#include "myintl.h"
#include "y2cc_globals.h"


/* Hope it's no longer quick&dirty ;-) */

YastModules::YastModules()
{
        configfile=0L;
	inputfile=0L;
	pos=0;
	modlist.setAutoDelete( TRUE );     // delete items when they are removed
	grouplist.setAutoDelete( TRUE );     // delete items when they are removed

	process=0L;
	firstline=true;
	linecount=0;
	nummodules=0;
	grp=0L;
	mod=0L;
	currentsection=Undef;
	preparedY2Exists=false;

	if(config.QuickStart == true)
	{
		QTimer::singleShot( 500, this, SLOT(prepareY2Start()));
	}
}

bool YastModules::init()
{
	if (getModList() != 0)
	{
		error=_("YaST2 was unable to create the list of available sections.\n\n"
			"This means that either YaST2 or this Control Center are\n"
			"not correctly installed, or that you don't have sufficient\n"
			"permissions.\n\n");
		error+=_("If the former describes your situation, re-install all YaST2 components;\n"
			"if the latter is the case,  try to run the Control Center as root. \n"
			"Also make sure that you do not run out of disk space. ");


		cerr << "*** " << error << " ***" << endl;
		return false;
	}
//	cerr <<  time(0) << " YastModules::init() -- end" << endl;
	return true;
}

YastModules::~YastModules()
{
        startPreparedY2(NULL);
	delete configfile;
	delete inputfile;
}

// searches for = in line, fills key and value with left and right side of the
// = without whitspace, returns false if line didn't contain a =
bool YastModules::KeyValue(QString line, QString& key, QString &value)
{
	int pos = line.find('=');
	if( pos < 0 )
	{
		key="";
		value="";
		return false;
	}
	else
	{
		key=line.mid(0,pos).stripWhiteSpace();
		value=line.mid(pos+1).stripWhiteSpace();
		// strip _(" ")
		if(value.startsWith("_(\"") && value.right(2)=="\")")
		{
			value=value.mid(3,value.length()-5);
		}
	}
	return true;
}


//prepares the "fast-y2module-start" feature
// it starts a y2base, which waits for a module name.
// Later on, when the user has choosen amodule, the
// prestarted y2base gets this name via pipe and starts the module
// then very fast

int my_system ( const char *command, int fd)
{
	int pid, status;

	if (!command)
		return 1;
	pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) //child
	{
		close (fd);
		// Thats the needed close, so that the other
		// process can close the pipe

		char *argv[4];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = strdup(command);
		argv[3] = 0;
		execv("/bin/sh", argv);
		exit(127);
	}
	do
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			if (errno != EINTR)
			return -1;
		}
		else
		{
			return status;
		}
	} while(1);
}


void YastModules::prepareY2Start()
{
//	qDebug(QString("preparing YaST2 quick start"));

	if (pipe (preparefd) == -1)
	{
		cerr << strerror(errno) << endl;
		return;
	}

	QString cmd = "/sbin/yast2 pre-start %1 &";
	cmd = cmd.arg (preparefd[0]);
	my_system (cmd, preparefd[1]);

	preparedY2Exists = true;

	return;
}



bool YastModules::startPreparedY2( const YastModule* module )
{
	if ( preparedY2Exists )
	{
		int ret;
		QString command;
		if(module)
		{
			QString ycpname = module->getYCPName();
			QString args = module->getArguments();
			command=ycpname + " " + args;
		}
		else
		{
			command="terminateY2Base";
		}

		preparedY2Exists = false;

//		qDebug(QString("quickstarting ")+command);
		ret = write (preparefd[1], (const char *)command, strlen((const char *)command)+1);

		close (preparefd[1]);

		if( ret < 0 ) return false;
	}

	return true;
}



//parses line, inserts groups and modules
void YastModules::parseonelinefromprocess(QString line)
{
	line=line.stripWhiteSpace();

	// empty lines and comments are ignored
	if(line.isEmpty()) return;
	if(line.startsWith(";")) return;
	if(line.startsWith("#")) return;

	if(line.startsWith("[Y2Module"))
	{
		addgrpandmod();
		if(line.right(1)=="]")
		{
			currentsection=Module;
			if(!mod) mod=new YastModule();
			// strlen("[Y2Module")+strlen("]")
			//                  9           1 = 10
			line=line.mid(9,line.length()-10);
			line=line.stripWhiteSpace();
			mod->setycpname(line);
		}
		else
		{
			currentsection=Undef;
		}
	}
	else if(line.startsWith("[Y2Group"))
	{
		addgrpandmod();
		if(line.right(1)=="]")
		{
			currentsection=Group;
			if (!grp) grp=new ModGroup();
			// strlen("[Y2Group")+strlen("]")
			//                  8           1 = 9
			line=line.mid(8,line.length()-9);
			line=line.stripWhiteSpace();
			grp->setRawName(line);
		}
		else
		{
			currentsection=Undef;
		}
	}
	else if(currentsection==Module)
	{
		// split line in key value pair
		if(!KeyValue(line,key,value))
		{
			qDebug(QString("Invalid Line")+line);
			return;
		}

		if(!key || !value)
			return;

		if ( key == "Name" )
		{
			mod->setname(value);
		}
		else if ( key == "Group" )
		{
			mod->setgroup(value);
		}
		else if ( key == "Icon" )
		{
			mod->seticon(value);
		}
		else if ( key == "Helptext" )
		{
			mod->setdescription(value);
		}
		else if ( key == "Arguments" )
		{
//			value.replace(QRegExp("^\\["), "" );
//			value.replace(QRegExp("\\]$"), "" );
			value.stripWhiteSpace();
			if (!value.isEmpty())
			{
				mod->setArguments(value);
			}
		}
		else if ( key == "Geometry" )
		{
			mod->setGeometry(value);
		}
		else if ( key == "SortKey" )
		{
			mod->setSortKey(value);
		}
		else if ( key == "Textdomain" )
		{
			mod->setTextdomain(value);
		}
		else if ( key == "RequiresRoot" )
		{
			mod->setRootFlag((value=="false"?false:true));
		}
		else
		{
//			qDebug(QString("Unknown Keyword: ")+line);
		}
	}
	else if(currentsection==Group)
	{
		// split line in key value pair
		if(!KeyValue(line,key,value))
		{
			qDebug(QString("Invalid Line")+line);
			return;
		}

		if(!key || !value)
			return;

		if ( key == "Name" )
		{
			grp->setName(value);
		}
		else if ( key == "Icon" )
		{
			grp->setIcon(value);
		}
		else if ( key == "SortKey" )
		{
			grp->setSortKey(value);
		}
	}
}

/*
  in case mod or grp pointers are != NULL, they are added to the module and
  grouplist resp.. Texts are translated before doing so.
*/

void YastModules::addgrpandmod()
{
	if(mod)
	{
		this->addModule(mod);
		mod=0L;
	}
	if(grp)
	{
		this->addGroup(grp);
		grp=0L;
	}
}


void YastModules::addModule( YastModule* module )
{
		QString domain(module->getTextdomain());
		if(!domain.isEmpty())
		{
			// switch textdomain to that of the menuentry for translation
			set_textdomain (domain);

			module->setdescription(QString(_(module->getDesc())).simplifyWhiteSpace());

			module->setname(_(module->getName()));
			module->seticon(_(module->getIcon()));

			set_textdomain (config.textdomain);
		}
		if((config.isroot == false && module->getRootFlag() == false) || config.isroot == true)
		{
			ModGroup* tmpgrp;
			tmpgrp=new ModGroup(module->getGroup());
			if(grouplist.find(tmpgrp)<0)
			{
				// group did not exist
				qDebug("Warning: new Group detected for Module " +
				       module->getName() + ", misspelled in y2cc file?");
				tmpgrp->setIcon("defaultgroup.png");
				tmpgrp=new ModGroup(module->getGroup());
				tmpgrp->addModule(module);
				addGroup(tmpgrp);
			}
			else
			{
				grouplist.current()->addModule(module);
				delete tmpgrp;
				tmpgrp=0L;
			}
			grouplist.first();

			modlist.append(module);
		}
}

void
YastModules::addGroup (ModGroup* group)
{
    // switch textdomain to that of the group for translation
    set_textdomain ("base");

    group->setName (_(group->getName ()));
    grouplist.append (group);

    set_textdomain (config.textdomain);
}

int YastModules::numGroups()
{
	return grouplist.count();
}

ModGroup* YastModules::firstGroup()
{
	return grouplist.first();
}

ModGroup* YastModules::nextGroup()
{
	return grouplist.next();
}

ModGroup* YastModules::setGroup(uint idx)
{
	return grouplist.at(idx);
}

ModGroup* YastModules::getGroup() const
{
	return grouplist.current();
}

void YastModules::dumpmods()
{
	YastModule* p;
	cout << "Available Modules:" << endl;
	for (p=modlist.first();p;p=modlist.next())
	{
		cout << "Name: " << p->getName() << ", Group: " << p->getGroup() << endl;
	}
}

void YastModules::dumpgroups()
{
	ModGroup* p;
	for (p=grouplist.first();p;p=grouplist.next())
	{
		cout << "Name: " << p->getName() << " icon: " << p->getIcon() << " key: " << p->getSortKey() << endl;
	}
}

bool YastModules::ReadFile( const QString filename)
{
	QFile file;

	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);

	QString buffer;

	file.setName(filename);
	// qDebug(file.name());
	if(file.open(IO_ReadOnly))
	{
		while ((buffer=stream.readLine()).isNull()==false)
		{
			parseonelinefromprocess(buffer);
		}
		if (mod || grp)
		{
			addgrpandmod();
		}
		file.close();
	}
	else
	{
		// %1 = filename
		emit sig_error(QString(_("Could not open %1")).arg(file.name()));
		return false;
	}
	return true;
}

int YastModules::getModList()
{
	QDir dir (CONFIGDIR "/", "*.y2cc");

	if (!dir.exists())
	{
		emit sig_error(QString(_("The directory %1 does not exist")).arg(CONFIGDIR));
		return 1;
	}

	QStringList files=dir.entryList();
	nummodules=dir.count();
	// qDebug("Number of Menuentries: " +QString("%1").arg(nummodules));

	mod = 0L;
	grp = 0L;

	this->ReadFile (CONFIGDIR "/y2cc.groups");

	for( QStringList::Iterator menuentry=files.begin(); menuentry != files.end(); menuentry++)
	{
		this->ReadFile(CONFIGDIR "/" + *menuentry);
	}

	// connect(process, SIGNAL(oneline(QString)), this, SLOT(parseonelinefromprocess(QString)));
	// connect(process, SIGNAL(finished(int)), this, SLOT(finish(int)));
	finish(0);

	return 0;
}

void YastModules::finish(int code)
{
	addgrpandmod();
//	clog << "sort grouplist" << endl;
	grouplist.sort();
//	clog << "sort modlist" << endl;
	modlist.sort();
	emit sig_finished(code);
}

void YastModules::runModule( const YastModule* module)
{
	if(!module)
	{
		cerr << "got NULL pointer in " << __FILE__ << " line " << __LINE__ << endl;
		return;
	}

	QString geo = module->getGeometry();

	if (!geo.isEmpty())
	{
		qDebug("setting geometry is not supported by quickstart, fallback to normal call");
		startPreparedY2( NULL );
	}

	if ( preparedY2Exists )
	{
		if(startPreparedY2( module ) == true ) return;
	}

	QString ycpname = module->getYCPName();
	QString args = module->getArguments();

/* bad idea, because Y2_GEOMETRY could have been set before
   so i should restore it afterwards, but I'm too lazy */
/*
	if(!geo.isEmpty())
	{
		geo=QString("Y2_GEOMETRY=")+geo;
		char* cgeo=new char[strlen(geo.latin1())+1];
		strcpy(cgeo,geo.latin1());
		int ret=putenv(cgeo);
		if(ret<0)
		{
			cerr << "putenv failed: " << ret << endl;
		}
	}
*/
	QString cmd;

	// let the shell do the work
	if (!geo.isEmpty())
	{
		cmd += "Y2_GEOMETRY=\"" + geo + "\" ";
	}
	cmd += "/sbin/yast2 ";
	cmd += ycpname;
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

// see comment above
//	unsetenv("Y2_GEOMETRY");
}


const QString* YastModules::getErrorString() const
{
	return &error;
}

/**********************/

YastModule::YastModule ()
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

YastModule::YastModule(const YastModule&)
{
	cerr << "Copy constructor" << endl;
	exit(1);
}

YastModule::YastModule(const QString& Name,const QString& YCPName,const QString& Group,const QString& Description,const QString& IconFileName)
{
	name=Name;
	ycpname=YCPName;
	description=Description;
	iconfilename=IconFileName;
	group=Group;
	requiresroot=true;
}

YastModule::~YastModule()
{
}
void YastModule::setname(const QString& Name)
{
	name=Name;
}
void YastModule::setycpname(const QString& YCPName)
{
	ycpname=YCPName;
}
void YastModule::setArguments(const QString& args)
{
	arguments=args;
}
void YastModule::setGeometry(const QString& geo)
{
	geometry=geo;
}
void YastModule::setSortKey(const QString& key)
{
	sortkey=key;
}
void YastModule::setTextdomain(const QString& key)
{
	textdomain=key;
}
void YastModule::setgroup(const QString& Group)
{
	group=Group;
}
void YastModule::setdescription(const QString& Description)
{
	description=Description;
}
void YastModule::seticon(const QString& IconFileName)
{
	iconfilename=IconFileName;
}
void YastModule::setRootFlag(bool yes)
{
	requiresroot=yes;
}

QString YastModule::getName() const
{
	return name;
}
QString YastModule::getYCPName() const
{
	return ycpname;
}
QString YastModule::getArguments() const
{
	return arguments;
}
QString YastModule::getGeometry() const
{
	return geometry;
}
QString YastModule::getSortKey() const
{
	return sortkey;
}
QString YastModule::getTextdomain() const
{
	return textdomain;
}
QString YastModule::getGroup() const
{
	return group;
}
QString YastModule::getDesc() const
{
	return description;
}
QString YastModule::getIcon() const
{
	return iconfilename;
}
bool YastModule::getRootFlag() const
{
	return requiresroot;
}
bool YastModule::operator<(const YastModule& b)
{
	bool value; //=(group<b.group);

	if(sortkey.isEmpty() && b.sortkey.isEmpty())
	{
		value=(name<b.name);
	}
	else if(sortkey.isEmpty() && !b.sortkey.isEmpty())
	{
		value=(name<b.sortkey);
	}
	else if(!sortkey.isEmpty() && b.sortkey.isEmpty())
	{
		value=(sortkey<b.name);
	}
	else
	{
		if(sortkey!=b.sortkey)
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
bool YastModule::operator==(const YastModule& b)
{
	bool value;//=(group==b.group);

	if(sortkey.isEmpty() && b.sortkey.isEmpty()) value=(name==b.name);
	else if(sortkey.isEmpty() && !b.sortkey.isEmpty()) value=(name==b.sortkey);
	else if(!sortkey.isEmpty() && b.sortkey.isEmpty()) value=(sortkey==b.name);
	else
	{
		if(sortkey!=b.sortkey)
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
	//class YastModules holds the primary pointer to the Data
	modules.setAutoDelete(false);
	sorted=true;
	icon="defaultgroup.png";
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
	if(this->name.isEmpty()) return this->rawname;
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
void ModGroup::addModule(YastModule* Module)
{
	modules.append(Module);
	sorted=false;
}
YastModule* ModGroup::first()
{
	if(sorted==false) {modules.sort(); sorted=true;}
	return modules.first();
}
YastModule* ModGroup::last()
{
	if(sorted==false) {modules.sort(); sorted=true;}
	return modules.last();
}
YastModule* ModGroup::prev()
{
	if(sorted==false) {modules.sort(); sorted=true;}
	return modules.prev();
}
YastModule* ModGroup::next()
{
	if(sorted==false) {modules.sort(); sorted=true;}
	return modules.next();
}
YastModule* ModGroup::current()
{
	if(sorted==false) {modules.sort(); sorted=true;}
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
		if(name.isEmpty())
			A=rawname;
		else
			A=name;
	}
	else
		A=sortkey;

	if (b.sortkey.isEmpty())
	{
		if(b.name.isEmpty())
			B=b.rawname;
		else
			B=b.name;
	}
	else
		B=b.sortkey;

	if( A==B && A==sortkey && B==b.sortkey )
	{
		if(name.isEmpty() || b.name.isEmpty())
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
bool ModGroup::operator>(const ModGroup& b)
{
	QString A,B;

	if (sortkey.isEmpty())
	{
		if(name.isEmpty())
			A=rawname;
		else
			A=name;
	}
	else
		A=sortkey;

	if (b.sortkey.isEmpty())
	{
		if(b.name.isEmpty())
			B=b.rawname;
		else
			B=b.name;
	}
	else
		B=b.sortkey;

	if( A==B && A==sortkey && B==b.sortkey )
	{
		if(name.isEmpty() || b.name.isEmpty())
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
	if(name.isEmpty() || b.name.isEmpty())
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
 *  class YastModuleData
 */

YastModuleData::YastModuleData()
{
	module=0L;
}

YastModuleData::~YastModuleData()
{
}

void YastModuleData::setModule(const YastModule* m)
{
	module=m;
}

const YastModule* YastModuleData::getModule() const
{
	return module;
}

#include <yastmodules.moc.cpp>
