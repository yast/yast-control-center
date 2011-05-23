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

