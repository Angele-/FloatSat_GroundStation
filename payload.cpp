#include "payload.h"

PayloadSensorData::PayloadSensorData(PayloadSatellite payload):roll(0.0f), pitch(0.0f), yaw(0.0f){
    if(payload.userDataLen != sizeof(PayloadSensorData) || payload.topic != PayloadSensorDataType)
        return;

    roll = *(float*)(payload.userData + 0 * sizeof(float));
    pitch = *(float*)(payload.userData + 1 * sizeof(float));
    yaw = *(float*)(payload.userData + 2 * sizeof(float));
}
