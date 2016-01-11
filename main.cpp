#include <QApplication>
#include "basics.h"
#include "groundstation.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GroundStation w;
    qInstallMessageHandler((QtMessageHandler)&w.logHandler);
    w.setFixedSize(1300, 690);
    w.show();

    return a.exec();
}
