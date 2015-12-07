#include "udptest.h"

UDPTestRead::UDPTestRead(QObject *parent) : QObject(parent)
{
    socket.bind(31337);
    connect(&socket, SIGNAL(readyRead()), this, SLOT(print()));
}

void UDPTestRead::print(){
    char buffer[1024];
    socket.readDatagram(buffer, 1023);
    qDebug() << buffer << "\n";
}
