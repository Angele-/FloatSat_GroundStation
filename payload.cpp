#include "payload.h"

PayloadSensorFusion::PayloadSensorFusion(PayloadSatellite payload):roll(-qInf()), pitch(-qInf()), yaw(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensorFusion) || payload.topic != PayloadSensorFusionType)
        return;

    roll = *(float*)(payload.userData + 0 * sizeof(float));
    pitch = *(float*)(payload.userData + 1 * sizeof(float));
    yaw = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensor1::PayloadSensor1(PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor1) || payload.topic != PayloadSensor1Type)
        return;

    x = *(float*)(payload.userData + 0 * sizeof(float));
    y = *(float*)(payload.userData + 1 * sizeof(float));
    z = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensor2::PayloadSensor2(PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor2) || payload.topic != PayloadSensor2Type)
        return;

    x = *(float*)(payload.userData + 0 * sizeof(float));
    y = *(float*)(payload.userData + 1 * sizeof(float));
    z = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadSensor3::PayloadSensor3(PayloadSatellite payload):x(-qInf()), y(-qInf()), z(-qInf()){
    if(payload.userDataLen != sizeof(PayloadSensor3) || payload.topic != PayloadSensor3Type)
        return;

    x = *(float*)(payload.userData + 0 * sizeof(float));
    y = *(float*)(payload.userData + 1 * sizeof(float));
    z = *(float*)(payload.userData + 2 * sizeof(float));
}

PayloadLight::PayloadLight(PayloadSatellite payload):light(0){
    if(payload.userDataLen != sizeof(PayloadLight) || payload.topic != PayloadLightType)
        return;

    light = *(quint16*)(payload.userData + 0 * sizeof(quint16));
}
