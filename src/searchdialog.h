/***************************************************************************
                              searchdialog.h
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    Copyright		: (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

// include files for QT
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qstatusbar.h>


class SearchDialog : public QDialog
{
  Q_OBJECT
  public:
    SearchDialog(QWidget *parent);
    ~SearchDialog();
    QString getText() const;
    void SearchResult ( QListBoxItem * box );
    void ClearResults();
    void message(const QString & msg,int time=0);
    int exec();

  signals:
    void sigSearch(QString text);

  protected slots:

    void slotbye(QListBoxItem *);
    void slotsearch();

  private:
    QLineEdit *txt;
    QVBoxLayout *vbox;
    QListBox *lb;
    QStatusBar *sb;

};

#endif
