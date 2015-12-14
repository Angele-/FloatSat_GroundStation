#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>

enum PayloadType{
    PayloadCounterType = 1,
    PayloadSensorDataType = 1001,
    PayloadLightType = 1002,
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

struct PayloadSensorData{
    float roll;
    float pitch;
    float yaw;
    PayloadSensorData(PayloadSatellite payload);
};

#endif // PAYLOAD_H

