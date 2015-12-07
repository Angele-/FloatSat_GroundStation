#include "groundstation.h"
#include <QApplication>

#include "udptest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //GroundStation w;
    //w.show();

    UDPTest x(UDP_WRITE);
    x.executeIfWriteMode();
    return a.exec();

    return 0;
}
