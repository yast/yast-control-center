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


#include <qfile.h>
#include <qstring.h>
#include <qlist.h>
#include <qvector.h>
#include <qsortedlist.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qobject.h>
#include "y2cc_globals.h"

class YMod;
class ModGroup;

typedef QSortedList<YMod>		ModuleList;
typedef QPtrListIterator<YMod>		ModuleListIterator;
    
typedef QSortedList<ModGroup>		GroupList;
typedef QPtrListIterator<ModGroup>	GroupListIterator;


class YModules: public QObject
{
    Q_OBJECT
    
public:
    YModules();
    virtual ~YModules();

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
    
    // Run a module with YaST2
    void runModule(const YMod * module);

    // Debugging
    void dumpModules();
    void dumpGroups();

    // Remove groups that don't contain any modules
    void removeEmptyGroups();

    const QString* getErrorString() const;

signals:

    void modulesReady();
    void modError( QString msg );

    
protected:

    /**
     * Initialize the module groups.
     * Returns 'true' on success.
     **/
    bool initGroups();

    /**
     * Initialize the modules.
     * Returns 'true' on success.
     **/
    bool initModules();
    
    /**
     * Initialize the language code (lang).
     * From de_DE@euro etc. only "de" is kept!
     **/
    void initLang();
    
    /**
     * Read one module .desktop file and add a module accordingly.
     * Returns 'true' on success.
     **/
    bool readModuleDesktopFile( const QString & path, const QString & filename );

    /**
     * Read one group .desktop file and add a module group accordingly.
     * Returns 'true' on success.
     **/
    bool readGroupDesktopFile( const QString & path, const QString & filename );

    /**
     * Add a module group (a category)
     **/
    void addGroup( ModGroup * group );

    /**
     * Add a module
     **/
    void addModule( YMod * mod );



    //
    // Data members
    //
    
    GroupList groupList;
    ModuleList modList;

    QString lang;
    QString langWithCountry;
    QString error;
    

    enum Sections
    {
	Undef,
	Group,
	Module
    };
};



class YMod
{
public:
    YMod();
    YMod(const QString& Name,const QString& YCPName,const QString& Group,const QString& Description,const QString& IconFileName);
    ~YMod();
    YMod(const YMod&);
    
    void setName(const QString& Name);
    void setYCPName(const QString& YCPName);
    void setDescription(const QString& Description);
    void setIcon(const QString& IconFileName);
    void setGroup(const QString& Group);
    void setArguments(const QString& args);
    void setGeometry(const QString& geo);
    void setSortKey(const QString& key);
    void setTextdomain(const QString& key);
    void setRootFlag(bool yes);
    
    QString getName() const;
    QString getYCPName() const;
    QString getDescription() const;
    QString getIcon() const;
    QString getGroup() const;
    QString getArguments() const;
    QString getGeometry() const;
    QString getSortKey() const;
    QString getTextdomain() const;
    bool getRootFlag() const;
    bool operator<(const YMod&);
    bool operator==(const YMod&);
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
    void addModule(YMod* Module);
    bool isEmpty() const;
    YMod* first();
    YMod* last();
    YMod* next();
    YMod* prev();
    YMod* current();
    
private:
    
    //List of modules that belong to this group
    
    ModuleList modules;
    QString name;
    QString icon;
    QString sortkey;
    QString rawname;
    bool sorted;
};

/*
 *  Carries only a pointer to a YMod, mainly used for DnD
 */
class YModData
{
	public:
	YModData();
	~YModData();
	void setModule( const YMod* );
	const YMod* getModule() const;
	private:
	const YMod* module;
};

#endif
