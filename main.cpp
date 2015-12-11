#include "groundstation.h"
#include <QApplication>

#include "satellitelink.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GroundStation w;
    w.show();

    return a.exec();

    return 0;
}
