#ifndef MYINTL_H
#define MYINTL_H


#include <libintl.h>
#include <qstring.h>


inline QString _(const char* msgid)
{
    return QString::fromUtf8 (gettext(msgid));
}

inline QString _(const char* msgid1, const char* msgid2, unsigned long int n)
{
    return QString::fromUtf8 (ngettext (msgid1, msgid2, n));
}

void set_textdomain (const char* domain);


#endif
