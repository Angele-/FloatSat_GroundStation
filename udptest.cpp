#include "udptest.h"

UDPTest::UDPTest(QObject *parent) : QObject(parent)
{
    if(socket.bind(31337)){
        qDebug() << "Bound!\n";
    }else{
        qDebug() << "Didn't bind!\n";
    }
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read()));
}

void UDPTest::print(){
    char buffer[1024];
    socket.readDatagram(buffer, 1023);
    qDebug() << buffer << "\n";
}

void UDPTest::write(QByteArray data){
    socket.write(data);
}
