#include <QApplication>
#include <QTimer>

#include "main_window.h"


int main(int argc, char **argv)
{
QApplication app(argc, argv);

MainWindow mainWin;

mainWin.show();

QTimer::singleShot(60, &mainWin, SLOT( initialMsg() ));

return app.exec();

}
