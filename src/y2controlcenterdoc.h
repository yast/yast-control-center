/***************************************************************************
                            y2controlcenterdoc.h
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    Copyright           : (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef Y2CONTROLCENTERDOC_H
#define Y2CONTROLCENTERDOC_H

// include files for QT
#include <qobject.h>

// application specific includes

/**
  * the Document Class
  */

class Y2ControlCenterDoc : public QObject
{
  Q_OBJECT

  public:
    Y2ControlCenterDoc();
    ~Y2ControlCenterDoc();
    void newDoc();
    bool save();
    bool saveAs(const QString &filename);
    bool load(const QString &filename);
    bool isModified() const;

  signals:
    void documentChanged();

  protected:
    bool modified;
};

#endif
