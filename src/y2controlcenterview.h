 /***************************************************************************
                            y2controlcenterview.h
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    Copyright           : (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/


#ifndef Y2CONTROLCENTERVIEW_H
#define Y2CONTROLCENTERVIEW_H

// include files for QT
#include <qapplication.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qiconview.h>
#include <qbuttongroup.h>
#include <qlistbox.h>
#include <qiconset.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qregexp.h>
#include "yastmodules.h"

#include <time.h>

// application specific includes
#include "myintl.h"

class SearchDialog;

class Y2ControlCenterView : public QWidget
{
    Q_OBJECT
    
public:
    Y2ControlCenterView(QWidget *parent=0);
    ~Y2ControlCenterView();
    bool init();
    const QString* getErrorString() const;
    signals:
    void quit();
    void sig_percentread(int percent);
    void statusmsg(const QString& msg);
    
protected:
    void runModule(const YMod *m);
    
protected slots:
    void slotButtonClicked(int);
    void slotquit();
    void slotresetcursor();
    void slothelp();
    void slotsearch();
    void slotsearchmodule(QString text);
    void slotIconClicked(QIconViewItem *);
    void slotOnItem(QIconViewItem *);
    void slot_percentread(int);
    void slotInitListbox(int);
    void slotAdjustListbox();
    void errorpopup(QString msg);
private:
    QBoxLayout* layout;
    QBoxLayout* hlayout;
    QBoxLayout* buttonhlayout;
    QIconView *icons;
    YModules * modules;
    QListBox *listbox;
    void filliconview(int groupnr);
    const QString * error;
    const YMod *lastCalledModule;
    time_t lastCallTime;
    time_t minwait;
    SearchDialog *sd;
    QVector<const YMod> *items;

};

#endif
