/***************************************************************************
            y2cc_config.c  -  stores global options
                             -------------------
    begin               : 25.06.2001
    Copyright           : (c) 2001 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#include "y2cc_config.h"

// for getuid
#include <unistd.h>
#include <sys/types.h>


Config::Config()
{
    isroot	= ( getuid() == 0 );
    QuickStart	= true;
    fullscreen	= false;
    noBorder 	= false;
}
