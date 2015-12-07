//#include "groundstation.h"
//#include <QApplication>

//#include "udptest.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //GroundStation w;
    //w.show();

    UDPTest x(UDP_READ);
    x.executeIfWriteMode();
    return a.exec();

    return 0;
}
