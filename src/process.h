/***************************************************************************
                                process.h
                             -------------------
    begin               : Tue Mar 13 13:30:00 CET 2001
    Copyright           : (c) 2001 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qtextstream.h>
#include <stdio.h>

class Process : public QObject
{
    Q_OBJECT
public:
    Process();
    ~Process();
    bool start(const QString& cmdline);
    int close();
    void clearbuffer();
    bool running();
    void setinterval(int usec);
    QString getbuffer() const;
signals:
    void oneline(QString line);
    void finished(int);
protected slots:
    void readfromprocess();
private:
    QString buffer;
    QTextStream* stream;
    FILE* fp;
    QTimer* timer;
    int interval;
};

#endif //PROCESS_H
