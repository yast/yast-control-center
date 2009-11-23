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
#include <qvbox.h>
#include <qpixmap.h>
#include "yastmodules.h"

#include <time.h>

// application specific includes
#include "myintl.h"

class SearchDialog;
class MyQIconView;
class QIconViewItem;
class QListBox;

class Y2ControlCenterView : public QVBox
{
    Q_OBJECT
    
public:
    Y2ControlCenterView(QWidget *parent=0);
    ~Y2ControlCenterView();
    bool init();
    const QString* getErrorString() const;

public slots:

    void search();
    void help();

signals:
    void statusMsg( const QString& msg );

    
protected slots:

    void slotInitListBox();
    void slotButtonClicked(int);
    void slotResetCursor();
    void slotSearchModule(QString text);
    void slotIconClicked(QIconViewItem *);
    void slotIconClicked(int button, QIconViewItem *item, const QPoint &pos);
    void slotOnItem(QIconViewItem *);
    void slotAdjustListbox();
    void errorPopup(QString msg);
    
protected:
    void runModule(const YMod *m);
    QWidget * layoutTitleBar( QWidget * parent );
    void setGradient( QWidget * widget, const QPixmap & pixmap );
    void fillIconView( int group_id );
    void addIconPath( const QString &dir );
    QString findIcon( QString icon );
    
private:
    
    YModules *		_modules;
    QVector<const YMod> *_items;
    
    QListBox *		_listBox;
    MyQIconView * 	_iconView;
    SearchDialog *	_searchDialog;

    QStringList icon_dirs;
    QMap <QString, QString> iconFileCache; 
    
    const QString * error;
    const YMod * lastCalledModule;
    time_t lastCallTime;
    time_t minwait;

};

#endif
