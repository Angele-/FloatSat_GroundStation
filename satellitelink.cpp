#include "satellitelink.h"
#include <QNetworkInterface>

SatelliteLink::SatelliteLink(QObject *parent) : QObject(parent), localAddress("0.0.0.0"), remoteAddress("192.168.1.255"), port(12345), socket(this), bound(false){
    qDebug() << "Binding to IP" << localAddress.toString() << "and port" << port << "\n";
    if(socket.bind(localAddress, port)){
        qDebug() << "Bind successful!\n";
        bound = true;
    }else{
        qDebug() << "Bind unsuccessful!\n";
        return;
    }
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read()));
}

void SatelliteLink::readFromSocket(){
    QByteArray buffer(1023, 0x00);
    socket.readDatagram(buffer.data(), buffer.size());

    for(int i = 0; i < buffer.size(); ++i){
        if(buffer[i] == (char)0xFF)
            buffer.remove(i+1, 1);
    }

    SatellitePayload payload;
    payload.checksum = *((quint16*)(buffer.constData() + 0));
    payload.senderNode = *((quint32*)(buffer.constData() + 2));
    payload.timestamp = *((quint64*)(buffer.constData() + 6));
    payload.senderThread = *((quint32*)(buffer.constData() + 14));
    payload.topic = *((quint32*)(buffer.constData() + 18));
    payload.ttl = *((quint16*)(buffer.constData() + 22));
    payload.userDataLen = *((quint16*)(buffer.constData() + 24));
    memcpy(payload.userData, buffer.constData() + 26, payload.userDataLen);
    payload.userData[payload.userDataLen] = 0x00;

    quint16 checksum = 0;
    for(int i = 2; i < 26 + payload.userDataLen; ++i){
        bool lowestBit = checksum & 1;
        checksum >>= 1;
        if(lowestBit)
            checksum |= 0x8000;

        checksum += buffer[i];
    }

    if(checksum == payload.checksum){
        payloads.enqueue(payload);
        emit readReady();
    }
}


bool SatelliteLink::write(SatellitePayload payload){
    QByteArray buffer(1023, 0x00);

    *((quint32*)(buffer.data() + 2)) = payload.senderNode;
    *((quint64*)(buffer.data() + 6)) = payload.timestamp;
    *((quint32*)(buffer.data() + 14)) = payload.senderThread;
    *((quint32*)(buffer.data() + 18)) = payload.topic;
    *((quint16*)(buffer.data() + 22)) = payload.ttl;
    *((quint16*)(buffer.data() + 24)) = payload.userDataLen;
    memcpy(buffer.data() + 26, payload.userData, payload.userDataLen);
    *(buffer.data() + 26 + payload.userDataLen) = 0x00;

    quint16 checksum = 0;
    for(int i = 2; i < 26 + payload.userDataLen; ++i){
        bool lowestBit = checksum & 1;
        checksum >>= 1;
        if(lowestBit)
            checksum |= 0x8000;

        checksum += buffer[i];
    }
    *((quint16*)(buffer.data() + 0)) = checksum;

    int extra = 0;
    for(int i = 0; i < 26 + payload.userDataLen + extra; ++i){
        if(buffer[i] == (char)0xFF){
            buffer.insert(i+1, 0x7E);
            ++i;
            ++extra;
        }
    }
    buffer.resize(26 + payload.userDataLen + extra);

    return socket.writeDatagram(buffer.constData(), remoteAddress, port);
}

SatellitePayload SatelliteLink::read(){
    if(!payloads.size())
        return SatellitePayload();

    return payloads.dequeue();
}

bool SatelliteLink::isBound(){
    return bound;
}

bool SatelliteLink::isReadReady(){
    return payloads.size();
}
