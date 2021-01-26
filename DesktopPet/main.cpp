#include "DesktopPet.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DesktopPet w;

    w.show();
    return a.exec();
}
