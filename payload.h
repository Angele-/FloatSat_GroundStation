#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>

enum PayloadType{
    PayloadCounterType = 1,
    PayloadSensorFusionType = 1001,
    PayloadLightType = 1002,
    PayloadSensor1Type = 1011,
    PayloadSensor2Type = 1012,
    PayloadSensor3Type = 1013,
};

struct PayloadSatellite{
    quint16 checksum;
    quint32 senderNode;
    quint64 timestamp;
    quint32 senderThread;
    quint32 topic;
    quint16 ttl;
    quint16 userDataLen;
    quint8 userData[998];
};

struct PayloadSensorFusion{
    float roll;
    float pitch;
    float yaw;
    PayloadSensorFusion(PayloadSatellite payload);
};

struct PayloadSensor1{
    float x;
    float y;
    float z;
    PayloadSensor1(PayloadSatellite payload);
};

struct PayloadSensor2{
    float x;
    float y;
    float z;
    PayloadSensor2(PayloadSatellite payload);
};

struct PayloadSensor3{
    float x;
    float y;
    float z;
    PayloadSensor3(PayloadSatellite payload);
};

struct PayloadLight{
    quint16 light;
    PayloadLight(PayloadSatellite payload);
};

#endif // PAYLOAD_H

