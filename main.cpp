#include "groundstation.h"
#include <QApplication>
#include <QTextStream>

#include "udptest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //GroundStation w;
    //w.show();

    UDPTest x;
    QTextStream s(stdin);
    while(true){
       x. write(s.readLine().toUtf8());
    }
    return a.exec();
}
