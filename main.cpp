#include "groundstation.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GroundStation w;
    w.show();

    return a.exec();
}
