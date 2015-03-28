#include "formstart.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(qApp->applicationDirPath() + "/plugins");

    FormStart w;
    w.show();
    
    return a.exec();
}
