/***************************************************************************
            yastmodules.h  -  Interface to ycp, handling of modules
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    Copyright           : (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef YASTMODULES_H
#define YASTMODULES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#define MODULEFILE "/tmp/y2controlcenter.moduleslist"

#include <qfile.h>
#include <qstring.h>
#include <qlist.h>
#include <qvector.h>
#include <qsortedlist.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qobject.h>
#include "y2cc_globals.h"

class YastModule;
class ModGroup;
class Process;

class YastModules : public QObject
{
	Q_OBJECT
	public:
	YastModules();
	~YastModules();

	//reads modules
	bool init();

	// return number of groups
        int numGroups();
	// jump to first group, return it
	ModGroup* firstGroup();
	// goto next group, return it
	ModGroup* nextGroup();
	// return current group
	ModGroup* getGroup() const;
	// jump to group #idx  and return it
	ModGroup* setGroup(uint idx);
	// position on first module
	/*
	void begin();
	// step to next module
	bool next();
	// return current module
	const YastModule* get() const;
	*/
	// run a module with yast2
	void runModule(const YastModule* module);

	void dumpmods();
	void dumpgroups();

	const QString* getErrorString() const;

	signals:

	void sig_percentread(int percent);
	void sig_finished(int);
	void sig_error(QString msg);

	protected slots:
	void parseonelinefromprocess(QString line);
	// add last module, sort groups and modules
        void finish(int);

	private:
	QSortedList<YastModule> modlist;
	QSortedList<ModGroup> grouplist;
//	QSortedList<QString> grouplist;
//	QVector<QString> groupicons;
	int pos;
	int getModList();
	QFile* inputfile;
	QTextStream *configfile;
	QString error;
	int nummodules;

	//config parsing
	void addgrpandmod();
	void addGroup( ModGroup* group );
	void addModule( YastModule* module );
	Process* process;
	bool firstline;
	int linecount;
	QString value;
	QString key;
	ModGroup* grp;
	YastModule* mod;
	bool KeyValue(QString line, QString& key, QString &value);
	bool ReadFile(const QString filename);

	enum Sections {Undef, Group, Module};
	Sections currentsection;

};

// ***

class YastModule
{
	public:
	YastModule();
	YastModule(const QString& Name,const QString& YCPName,const QString& Group,const QString& Description,const QString& IconFileName);
	~YastModule();
	YastModule(const YastModule&);
	void setname(const QString& Name);
	void setycpname(const QString& YCPName);
	void setdescription(const QString& Description);
	void seticon(const QString& IconFileName);
	void setgroup(const QString& Group);
	void setArguments(const QString& args);
	void setGeometry(const QString& geo);
	void setSortKey(const QString& key);
	void setTextdomain(const QString& key);
	void setRootFlag(bool yes);
	QString getName() const;
	QString getYCPName() const;
	QString getDesc() const;
	QString getIcon() const;
	QString getGroup() const;
	QString getArguments() const;
	QString getGeometry() const;
	QString getSortKey() const;
	QString getTextdomain() const;
	bool getRootFlag() const;
	bool operator<(const YastModule&);
	bool operator==(const YastModule&);
	private:
	QString name;
	QString ycpname;
	QString group;
	QString description;
	QString iconfilename;
	QString arguments;
	QString geometry;
	QString sortkey;
	QString textdomain;
	bool requiresroot;
};

class ModGroup
{
	public:
	ModGroup();
	ModGroup(const QString& Name);
	~ModGroup();
	QString getName() const;
	QString getRawName() const;
	QString getIcon() const;
	QString getSortKey() const;
	void setName(const QString& Name);
	void setRawName(const QString& Name);
	void setSortKey(const QString& Key);
	void setIcon(const QString& Icon);
	bool operator<(const ModGroup&);
	bool operator>(const ModGroup&);
	bool operator==(const ModGroup&);
	void addModule(YastModule* Module);
	bool isEmpty() const;
	YastModule* first();
	YastModule* last();
	YastModule* next();
	YastModule* prev();
	YastModule* current();
	private:
	//list of modules which belong to this group
	QSortedList<YastModule> modules;
	QString name;
	QString icon;
	QString sortkey;
	QString rawname;
	bool sorted;
};

/*
 *  Carries only a pointer to a YastModule, mainly used for DnD
 */
class YastModuleData
{
	public:
	YastModuleData();
	~YastModuleData();
	void setModule( const YastModule* );
	const YastModule* getModule() const;
	private:
	const YastModule* module;
};

#endif
