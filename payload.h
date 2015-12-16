#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>

enum PayloadType{
    PayloadCounterType = 9999,
    PayloadSensorFusionType = 1001,
    PayloadLightType = 1002,
    PayloadSensorGyroType = 1003,
    PayloadSensorXMType = 1004,
    PayloadSensor1Type = 1011,
    PayloadSensor2Type = 1012,
    PayloadSensor3Type = 1013,
};

struct PayloadSensorFusion;
struct PayloadSensor1;
struct PayloadSensor2;
struct PayloadSensor3;
struct PayloadLight;

struct PayloadSatellite{
    quint16 checksum;
    quint32 senderNode;
    quint64 timestamp;
    quint32 senderThread;
    quint32 topic;
    quint16 ttl;
    quint16 userDataLen;
    quint8 userData[998];
    PayloadSatellite();
    PayloadSatellite(const QByteArray &buffer);
    PayloadSatellite(const PayloadSensorFusion payload);
    PayloadSatellite(const PayloadSensor1 payload);
    PayloadSatellite(const PayloadSensor2 payload);
    PayloadSatellite(const PayloadSensor3 payload);
    PayloadSatellite(const PayloadLight payload);
};

struct PayloadSensorFusion{
    float roll;
    float pitch;
    float yaw;
    PayloadSensorFusion(const PayloadSatellite payload);
};

struct PayloadSensorGyro{
    float roll;
    float pitch;
    float yaw;
    PayloadSensorGyro(const PayloadSatellite payload);
};

struct PayloadSensorXM{
    float roll;
    float pitch;
    float yaw;
    PayloadSensorXM(const PayloadSatellite payload);
};

struct PayloadSensor1{
    float x;
    float y;
    float z;
    PayloadSensor1(const PayloadSatellite payload);
};

struct PayloadSensor2{
    float x;
    float y;
    float z;
    PayloadSensor2(const PayloadSatellite payload);
};

struct PayloadSensor3{
    float x;
    float y;
    float z;
    PayloadSensor3(const PayloadSatellite payload);
};

struct PayloadLight{
    quint16 light;
    PayloadLight(const PayloadSatellite payload);
};

#endif // PAYLOAD_H

