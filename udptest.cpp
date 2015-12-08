#include "udptest.h"
#include <QNetworkInterface>

UDPTest::UDPTest(UDPMode mode, QObject *parent) : QObject(parent), mode(mode), address("239.255.43.21"), port(31337)
{
    switch(mode){
    case UDP_READ:
        qDebug() << "Read mode. Binding to port 31337.\n";
        if(socket.bind(QHostAddress::AnyIPv4, 31337, QUdpSocket::ShareAddress)){
            qDebug() << "Bind successful!\n";
        }else{
            qDebug() << "Bind unsuccessful!\n";
            return;
        }
        if(socket.joinMulticastGroup(address)){
            qDebug() << "Multicast join successful!\n";
        }else{
            qDebug() << "Multicast join unsuccessful!\n";
            return;
        }
        connect(&socket, SIGNAL(readyRead()), this, SLOT(print()));
        break;
    case UDP_WRITE:
        qDebug() << "Write mode. IP addres, port 31337.";
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
    while(true) qDebug() << "Written " << socket.writeDatagram(in.readLine().toUtf8(), address, port) << " bytes\n";
}
