/***************************************************************************
            y2cc_config.h  -  stores global options
                             -------------------
    begin               : 25.06.2001
    Copyright           : (c) 2001 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef Y2CC_CONFIG_H
#define Y2CC_CONFIG_H

#include "qstring.h"

class Config
{
public:
    bool isroot;
    bool fullscreen;
    bool noBorder;
    QString textdomain;

    // Constructor
    Config();
};

#endif
