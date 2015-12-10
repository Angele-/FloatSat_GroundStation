#include "udptest.h"
#include <QNetworkInterface>

UDPTest::UDPTest(UDPMode mode, QObject *parent) : QObject(parent), mode(mode), localAddress("0.0.0.0"), multiAddress("192.168.1.255"), port(12345), socket(this){
    switch(mode){
    case UDP_READ:
        qDebug() << "Read mode. Binding to port" << port << "\n";
        if(socket.bind(localAddress, port)){
            qDebug() << "Bind successful!\n";
        }else{
            qDebug() << "Bind unsuccessful!\n";
            return;
        }
        connect(&socket, SIGNAL(readyRead()), this, SLOT(print()));
        break;
    case UDP_WRITE:
        qDebug() << "Write mode. Port " << port;
        break;
    }
}

void UDPTest::print(){
    char buffer[1024];
    socket.readDatagram(buffer, 1023);
    qDebug() << buffer << "\n";
}

void UDPTest::executeIfWriteMode(){
    if(mode != UDP_WRITE)
        return;

    QTextStream in(stdin);
    while(true) qDebug() << "Written " << socket.writeDatagram(in.readLine().toUtf8(), multiAddress, port) << " bytes\n";
}
