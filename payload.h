#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <QtGlobal>
#include "basics.h"

#define USER_DATA_MAX_LEN 992
#define PICTURE_WIDTH 160
#define PICTURE_HEIGHT 120

enum PayloadType{
    PayloadCounterType = 9999,
    PayloadSensorFusionType = 1001,
    PayloadLightType = 1002,
    PayloadSensorGyroType = 1003,
    PayloadSensorXMType = 1004,
    PayloadCameraPropertiesType = 1005,
    PayloadCameraPixelType = 1006,
    PayloadMeasurementsType = 1007,
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
    union{
        quint8 userData8[USER_DATA_MAX_LEN / sizeof(quint8)];
        quint16 userData16[USER_DATA_MAX_LEN / sizeof(quint16)];
        quint32 userData32[USER_DATA_MAX_LEN / sizeof(quint32)];
        quint64 userData64[USER_DATA_MAX_LEN / sizeof(quint64)];
        float userDataFloat[USER_DATA_MAX_LEN / sizeof(float)];
        double userDataDouble[USER_DATA_MAX_LEN / sizeof(double)];
    };
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

struct PayloadMeasurements{
    float batteryCurrent, batteryVoltage;
    float panelVoltage;
    float motorCurrent;
    float servo1, servo2, servo3, servo4;

    PayloadMeasurements():batteryCurrent(0), batteryVoltage(0), panelVoltage(0), motorCurrent(0), servo1(0), servo2(0), servo3(0), servo4(0){}
    PayloadMeasurements(const PayloadSatellite payload);
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

struct Telecommand{
    float command;
    quint16 satellite;
    quint16 thread;
    quint32 variableId;
    Telecommand(float command, quint16 satellite, quint16 thread, quint32 variableId);
};

struct Pixel{
    quint8 r,g,b;
    Pixel():r(0),g(0),b(0) {}
    Pixel(const PayloadSatellite payload);
};

struct PixelRow{
    Pixel pixel[ROWSIZE];
    PixelRow(const PayloadSatellite payload);
};
struct PictureProperties{
    quint16 Width;
    quint16 Height;
    quint8 type;
    PictureProperties(const PayloadSatellite payload);
    PictureProperties(): Width(0), Height(0) {}
};

#endif // PAYLOAD_H

