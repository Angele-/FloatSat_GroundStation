#include <QByteArray>
#include <QtEndian>
#include "payload.h"
#include <QDebug>

PayloadSatellite::PayloadSatellite() : checksum(0), senderNode(0), timestamp(0), senderThread(0), topic(0), ttl(0), userDataLen(0){
    userData8[0] = 0;
}

PayloadSatellite::PayloadSatellite(const QByteArray &buffer) : checksum(0), senderNode(0), timestamp(0), senderThread(0), topic(0), ttl(0), userDataLen(0){
    userData8[0] = 0;
    if(buffer.size() < 1023)
        return;

    checksum = qFromBigEndian(*((quint16*)(buffer.constData() + 0)));
    senderNode = qFromBigEndian(*((quint32*)(buffer.constData() + 2)));
    timestamp = qFromBigEndian(*((quint64*)(buffer.constData() + 6)));
    senderThread = qFromBigEndian(*((quint32*)(buffer.constData() + 14)));
    topic = qFromBigEndian(*((quint32*)(buffer.constData() + 18)));
    ttl = qFromBigEndian(*((quint16*)(buffer.constData() + 22)));
    userDataLen = qFromBigEndian(*((quint16*)(buffer.constData() + 24)));
    memcpy(userData8, buffer.constData() + 26, userDataLen);
    userData8[userDataLen] = 0x00;
}

PayloadSensorFusion::PayloadSensorFusion(const PayloadSatellite payload):roll(0.0), pitch(0.0), yaw(0.0){
    if(payload.userDataLen != sizeof(PayloadSensorFusion) || payload.topic != PayloadSensorFusionType)
        return;

    roll = payload.userDataFloat[0];
    pitch = payload.userDataFloat[1];
    yaw = payload.userDataFloat[2];
}

PayloadSensorGyro::PayloadSensorGyro(const PayloadSatellite payload):roll(-qInf()), pitch(-qInf()), yaw(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensorGyro) || payload.topic != PayloadSensorGyroType)
        return;

    roll = payload.userDataFloat[0];
    pitch = payload.userDataFloat[1];
    yaw = payload.userDataFloat[2];
}

PayloadSensorXM::PayloadSensorXM(const PayloadSatellite payload):roll(-qInf()), pitch(-qInf()), yaw(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensorXM) || payload.topic != PayloadSensorXMType)
        return;

    roll = payload.userDataFloat[0];
    pitch = payload.userDataFloat[1];
    yaw = payload.userDataFloat[2];
}

PayloadSensor1::PayloadSensor1(const PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor1) || payload.topic != PayloadSensor1Type)
        return;

    x = payload.userDataFloat[0];
    y = payload.userDataFloat[1];
    z = payload.userDataFloat[2];
}

PayloadSensor2::PayloadSensor2(const PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor2) || payload.topic != PayloadSensor2Type)
        return;

    x = payload.userDataFloat[0];
    y = payload.userDataFloat[1];
    z = payload.userDataFloat[2];
}

PayloadSensor3::PayloadSensor3(const PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor3) || payload.topic != PayloadSensor3Type)
        return;

    x = payload.userDataFloat[0];
    y = payload.userDataFloat[1];
    z = payload.userDataFloat[2];
}

PayloadLight::PayloadLight(const PayloadSatellite payload):light(0){
    if(payload.userDataLen != sizeof(PayloadLight) || payload.topic != PayloadLightType)
        return;

    light = payload.userData16[0];
}

PictureProperties::PictureProperties(const PayloadSatellite payload): Width(0), Height(0){

    //qDebug() << "Act " << payload.userDataLen << " Exp " << sizeof(PictureProperties);
    if(payload.userDataLen != sizeof(PictureProperties) || payload.topic != PayloadCameraPropertiesType){
        return;
    }
    Height = payload.userData16[0];
    Width = payload.userData16[1];
    qDebug() << "W: " << Width << " H: " << Height << endl;
}

Pixel::Pixel(const PayloadSatellite payload): r(0), g(0), b(0){
    qDebug() << "Act " << payload.userDataLen << " Exp " << sizeof(PictureProperties);
    if(payload.userDataLen != sizeof(Pixel) || payload.topic != PayloadCameraPixelType){
        return;
    }

    r = payload.userData8[0];
    g = payload.userData8[1];
    b = payload.userData8[2];

    //qDebug() << "R " << r << " G " << g << " B " << b << endl;
}

PixelRow::PixelRow(const PayloadSatellite payload){
    //qDebug() << "Act " << payload.userDataLen << " Exp " << sizeof(PixelRow);

    if(payload.userDataLen != sizeof(PixelRow) || payload.topic != PayloadCameraPixelType){
        return;
    }

    for(int i = 0; i < ROWSIZE * 3; i++){
        pixel[i].r = payload.userData8[i];
        pixel[i].g = payload.userData8[i];
        pixel[i].b = payload.userData8[i];

        //qDebug() << pixel[i].r << endl;
    }


}

Telecommand::Telecommand(float command, quint16 satellite, quint16 thread, quint32 variableId) : command(command), satellite(satellite), thread(thread), variableId(variableId){
}
