#include "satellitelink.h"
#include <QNetworkInterface>
#include <QDateTime>
#include <QtEndian>

SatelliteLink::SatelliteLink(QObject *parent, bool checkChecksum) : QObject(parent), localAddress("0.0.0.0"), remoteAddress("192.168.1.255"), port(12345), socket(this), bound(false), checkChecksum(checkChecksum){
    qDebug() << "Binding to IP" << localAddress.toString() << "and port" << port << "\n";
    if(socket.bind(localAddress, port)){
        qDebug() << "Bind successful!\n";
        bound = true;
    }else{
        qDebug() << "Bind unsuccessful!\n";
        return;
    }
    connect(&socket, SIGNAL(readyRead()), this, SLOT(readFromSocket()));
}

void SatelliteLink::readFromSocket(){
    QByteArray buffer(1023, 0x00);
    socket.readDatagram(buffer.data(), buffer.size());

//    for(int i = 0; i < buffer.size(); ++i){
//        if(buffer[i] == (char)0xFF)
//            buffer.remove(i+1, 1);
//    }

    PayloadSatellite payload(buffer);

    quint16 checksum = 0;
    for(int i = 2; i < 26 + payload.userDataLen; ++i){
        bool lowestBit = checksum & 1;
        checksum >>= 1;
        if(lowestBit)
            checksum |= 0x8000;

        checksum += buffer[i];
    }

    if((!checkChecksum || checksum == payload.checksum) && topics.contains(payload.topic)){
        payloads.enqueue(payload);
        emit readReady();
    }
    if(payload.topic == PayloadSensorFusionType){
        PayloadSensorFusion psd(payload);
        QString roll, pitch, yaw;
        roll.sprintf("%+06.2f", psd.roll);
        pitch.sprintf("%+06.2f", psd.pitch);
        yaw.sprintf("%+06.2f", psd.yaw);
        qDebug() << qSetRealNumberPrecision(2) << "Roll:" << roll << "Pith:" << pitch << "Yaw:" << yaw;
    }
}

void SatelliteLink::addTopic(PayloadType topicId){
    topics.insert(topicId);
}

int SatelliteLink::write(quint32 topicId, const QByteArray &data){
    QByteArray buffer(1023, 0x00);

    *((quint32*)(buffer.data() + 2)) = qToBigEndian(0); // TODO payload.senderNode;
    *((quint64*)(buffer.data() + 6)) = qToBigEndian(QDateTime::currentDateTime().toMSecsSinceEpoch() * 1000000);
    *((quint32*)(buffer.data() + 14)) = qToBigEndian(0);
    *((quint32*)(buffer.data() + 18)) = qToBigEndian(topicId);
    *((quint16*)(buffer.data() + 22)) = qToBigEndian(64);
    *((quint16*)(buffer.data() + 24)) = qToBigEndian(data.length());
    memcpy(buffer.data() + 26, data.constData(), data.length());
    *(buffer.data() + 26 + data.length()) = 0x00;

    quint16 checksum = 0;
    for(int i = 2; i < 26 + data.length(); ++i){
        bool lowestBit = checksum & 1;
        checksum >>= 1;
        if(lowestBit)
            checksum |= 0x8000;

        checksum += buffer[i];
    }
    *((quint16*)(buffer.data() + 0)) = checksum;

//    int extra = 0;
//    for(int i = 0; i < 26 + data.length() + extra; ++i){
//        if(buffer[i] == (char)0xFF){
//            buffer.insert(i+1, 0x7E);
//            ++i;
//            ++extra;
//        }
//    }
//    buffer.resize(26 + data.length() + extra);

    return socket.writeDatagram(buffer.constData(), remoteAddress, port);
}

PayloadSatellite SatelliteLink::read(){
    if(!payloads.size())
        return PayloadSatellite();

    return payloads.dequeue();
}

bool SatelliteLink::isBound(){
    return bound;
}

bool SatelliteLink::isReadReady(){
    return payloads.size();
}
