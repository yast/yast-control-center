 /***************************************************************************
                                 resource.h
                             -------------------
    begin               : Mit Okt 18 14:21:09 CEST 2000
    Copyright           : (c) 2000 by SuSE GmbH
    author              : lnussel@suse.de
 ***************************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


///////////////////////////////////////////////////////////////////
// resource.h  -- contains macros used for commands


///////////////////////////////////////////////////////////////////
// COMMAND VALUES FOR MENUBAR AND TOOLBAR ENTRIES


///////////////////////////////////////////////////////////////////
// File-menu entries
#define ID_FILE_NEW                 10020
#define ID_FILE_OPEN                10030

#define ID_FILE_SAVE                10050
#define ID_FILE_SAVE_AS             10060
#define ID_FILE_CLOSE               10070

#define ID_FILE_PRINT               10080

#define ID_FILE_QUIT                10100


///////////////////////////////////////////////////////////////////
// Edit-menu entries
#define ID_EDIT_UNDO                11010
#define ID_EDIT_REDO                11020
#define ID_EDIT_COPY                11030
#define ID_EDIT_CUT                 11040
#define ID_EDIT_PASTE               11050
#define ID_EDIT_SELECT_ALL          11060


///////////////////////////////////////////////////////////////////
// View-menu entries                    
#define ID_VIEW_TOOLBAR             12010
#define ID_VIEW_STATUSBAR           12020

///////////////////////////////////////////////////////////////////
// Help-menu entries
#define ID_HELP_ABOUT               1002

///////////////////////////////////////////////////////////////////
// General application values
#define IDS_APP_ABOUT               "Y2ControlCenter\nVersion " VERSION \
                                    "\n(w) 2000 by SuSE GmbH"
#define IDS_STATUS_DEFAULT          ""

#endif // RESOURCE_H
