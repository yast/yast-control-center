#include "process.h"

//for select()
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define QTEXTSTREAM

Process::Process()
{
  fp=NULL;
  stream=0L;
  timer=0L;
  interval=500;
}

Process::~Process()
{
  if (fp) close();
  delete timer;
}

bool Process::start(const QString& cmdline)
{
  fp=popen(cmdline,"r");
  if (!fp)
  {
    qDebug("process not started");
    return false;
  }
#ifdef QTEXTSTREAM
  stream = new QTextStream(fp,IO_ReadOnly);
  stream->setEncoding(QTextStream::UnicodeUTF8);
#endif

  clearbuffer();

  timer = new QTimer( this );
  connect( timer, SIGNAL(timeout()), SLOT(readfromprocess()) );
  timer->start( 0, FALSE );

  return true;
}

int Process::close()
{
#ifdef QTEXTSTREAM
  delete stream;
  stream=0L;
#endif
  int ret=pclose(fp);
  qDebug(QString("process closed: %1").arg(ret));
  fp=NULL;
  delete timer;
  timer=0L;
  return ret;
}

void Process::setinterval(int usec)
{
  interval=usec;
}

bool Process::running()
{
  if (fp) return true;
  return false;
}

void Process::readfromprocess()
{
  fd_set rfds;
  struct timeval tv;
  int retval;
  int descriptor=fileno(fp);

  FD_ZERO(&rfds);
  FD_SET(descriptor, &rfds);
  
  tv.tv_sec = 0;
  tv.tv_usec = interval;

  retval = select(descriptor+1, &rfds, NULL, NULL, &tv);

  if (retval>0)
  {
#ifdef QTEXTSTREAM
    buffer=stream->readLine();
    if (buffer.isNull())
    {
      int ecode=close();
      emit finished(ecode);
    }
    else
    {
      emit oneline(buffer);
    }
#else
    char c=fgetc(fp);
    if (c==EOF)
    {
      int ecode=close();
      //last line had no newline
      if (!buffer.isEmpty())
      {
        buffer=QString::fromUtf8(buffer);
	emit oneline(buffer);
	clearbuffer();
      }
      emit finished(ecode);
      qDebug("eof");
    }
    else if (c=='\n')
    {
      buffer=QString::fromUtf8(buffer);
      emit oneline(buffer);
      clearbuffer();
    }
    else
    {
      buffer+=c;
    }
#endif
  }
  else if (retval==-1)
  {
    qDebug("select error");
  }
}

void Process::clearbuffer()
{
  buffer=(char*)0L;
}

QString Process::getbuffer() const
{
  return buffer;
}

#include "process.moc.cpp"

// vim:sw=2
