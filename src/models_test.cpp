 /****************************************************************************
|
| Copyright (c) 2011 Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
 \***************************************************************************/


#include <QApplication>
#include <QListView>

#include "yqdesktopfilesmodel.h"
#include "yqmodulesmodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QListView window;
    YQDesktopFilesModel model(&window);
    model.addIconPath("/usr/share/YaST2/theme/current/icons/32x32/apps");
    model.addDesktopFilesPath("/usr/share/applications/YaST2/groups");
    
    window.setModel(&model);
    window.show();

    //QListView window2;
    //YQDesktopFilesModel model2("/usr/share/applications/YaST2", &window2);
    //model2.addIconPath("/usr/share/YaST2/theme/current/icons/32x32/apps");
    //model2.addIconPath("/usr/share/icons/hicolor/32x32/apps");
    
    //window2.setModel(&model2);
    //window2.show();

    QListView window3;
    YQModulesModel model3(&window3);
    
    window3.setModel(&model3);
    window3.show();


    return app.exec();
}

