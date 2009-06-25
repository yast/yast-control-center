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

#ifndef KCMYAST_H
#define KCMYAST_H

#include <QStringList>

#include <kcmodule.h>

/**
 * Embeds the YaST main module list into
 * a KDE KCM module
 *
 * @author Duncan Mac-Vicar P. <dmacvicar@suse.de>
 */
class KCMYaST : public KCModule
{
    Q_OBJECT

public:
  /**
   * reimplemented from KCModule
   */
   KCMYaST(QWidget *parent, const QVariantList &args);
   ~KCMYaST();
   virtual void load();
   virtual void save();
   virtual void defaults();
};

#endif


