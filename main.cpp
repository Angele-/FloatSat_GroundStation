#include "groundstation.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GroundStation w;
    w.setFixedSize(1300, 690);
    w.show();

    return a.exec();
}
