#include <QByteArray>
#include <QtEndian>
#include "payload.h"

PayloadSatellite::PayloadSatellite() : checksum(0), senderNode(0), timestamp(0), senderThread(0), topic(0), ttl(0), userDataLen(0){
    userData[0] = 0;
}

PayloadSatellite::PayloadSatellite(const QByteArray &buffer) : checksum(0), senderNode(0), timestamp(0), senderThread(0), topic(0), ttl(0), userDataLen(0){
    userData[0] = 0;
    if(buffer.size() < 1023)
        return;

    checksum = qFromBigEndian(*((quint16*)(buffer.constData() + 0)));
    senderNode = qFromBigEndian(*((quint32*)(buffer.constData() + 2)));
    timestamp = qFromBigEndian(*((quint64*)(buffer.constData() + 6)));
    senderThread = qFromBigEndian(*((quint32*)(buffer.constData() + 14)));
    topic = qFromBigEndian(*((quint32*)(buffer.constData() + 18)));
    ttl = qFromBigEndian(*((quint16*)(buffer.constData() + 22)));
    userDataLen = qFromBigEndian(*((quint16*)(buffer.constData() + 24)));
    memcpy(userData, buffer.constData() + 26, userDataLen);
    userData[userDataLen] = 0x00;
}
//Not working
PayloadSensorFusion::PayloadSensorFusion(const PayloadSatellite payload):roll(0.0), pitch(-qInf()), yaw(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensorFusion) || payload.topic != PayloadSensorFusionType)
        return;

    roll = *(float*)(payload.userData + 0 * sizeof(float));
    pitch = *(float*)(payload.userData + 1 * sizeof(float));
    yaw = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensorGyro::PayloadSensorGyro(const PayloadSatellite payload):roll(-qInf()), pitch(-qInf()), yaw(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensorGyro) || payload.topic != PayloadSensorGyroType)
        return;

    roll = *(float*)(payload.userData + 0 * sizeof(float));
    pitch = *(float*)(payload.userData + 1 * sizeof(float));
    yaw = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensorXM::PayloadSensorXM(const PayloadSatellite payload):roll(-qInf()), pitch(-qInf()), yaw(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensorXM) || payload.topic != PayloadSensorXMType)
        return;

    roll = *(float*)(payload.userData + 0 * sizeof(float));
    pitch = *(float*)(payload.userData + 1 * sizeof(float));
    yaw = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensor1::PayloadSensor1(const PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor1) || payload.topic != PayloadSensor1Type)
        return;

    x = *(float*)(payload.userData + 0 * sizeof(float));
    y = *(float*)(payload.userData + 1 * sizeof(float));
    z = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensor2::PayloadSensor2(const PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor2) || payload.topic != PayloadSensor2Type)
        return;

    x = *(float*)(payload.userData + 0 * sizeof(float));
    y = *(float*)(payload.userData + 1 * sizeof(float));
    z = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensor3::PayloadSensor3(const PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor3) || payload.topic != PayloadSensor3Type)
        return;

    x = *(float*)(payload.userData + 0 * sizeof(float));
    y = *(float*)(payload.userData + 1 * sizeof(float));
    z = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadLight::PayloadLight(const PayloadSatellite payload):light(0){
    if(payload.userDataLen != sizeof(PayloadLight) || payload.topic != PayloadLightType)
        return;

    light = *(quint16*)(payload.userData + 0 * sizeof(quint16));
}

Telecommand::Telecommand(float command, quint16 satellite, quint16 thread) : command(command), satellite(satellite), thread(thread){
}
