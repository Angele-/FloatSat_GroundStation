#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"

GroundStation::GroundStation(QWidget *parent) :
    QMainWindow(parent), link(parent), ui(new Ui::GroundStation)
{
    link.addTopic(PayloadSensorFusionType);
    link.addTopic(PayloadSensorGyroType);
    link.addTopic(PayloadSensorXMType);
    link.addTopic(PayloadLightType);
    link.addTopic(PayloadCounterType);
    connect(&link, SIGNAL(readReady()), this, SLOT(readFromLink()));
    ui->setupUi(this);
}

void GroundStation::readFromLink(){
    PayloadSatellite payload = link.read();
    switch(payload.topic){
    case PayloadSensorFusionType:{
        PayloadSensorFusion psd(payload);
        this->findChild<QLCDNumber*>("lcdNumber_6")->display(psd.roll * 180.0f / M_PI);
        this->findChild<QLCDNumber*>("lcdNumber_8")->display(psd.pitch * 180.0f / M_PI);
        this->findChild<QLCDNumber*>("lcdNumber_10")->display(psd.yaw * 180.0f / M_PI);
        break;
    }
    case PayloadSensorGyroType:{
        PayloadSensorGyro psg(payload);
        this->findChild<QLCDNumber*>("lcdNumber_29")->display(psg.roll * 180.0f / M_PI);
        this->findChild<QLCDNumber*>("lcdNumber_30")->display(psg.pitch * 180.0f / M_PI);
        this->findChild<QLCDNumber*>("lcdNumber_31")->display(psg.yaw * 180.0f / M_PI);
        break;
    }
    case PayloadSensorXMType:{
        PayloadSensorXM psx(payload);
        this->findChild<QLCDNumber*>("lcdNumber_5")->display(psx.roll * 180.0f / M_PI);
        this->findChild<QLCDNumber*>("lcdNumber_7")->display(psx.pitch * 180.0f / M_PI);
        this->findChild<QLCDNumber*>("lcdNumber_9")->display(psx.yaw * 180.0f / M_PI);
        break;
    }
    case PayloadSensor1Type:{
        PayloadSensor1 ps(payload);
        break;
    }
    case PayloadSensor2Type:{
        PayloadSensor2 ps(payload);
        break;
    }
    case PayloadSensor3Type:{
        PayloadSensor3 ps(payload);
        break;
    }
    case PayloadLightType:{
        PayloadLight pl(payload);
        this->findChild<QLCDNumber*>("lcdNumber_32")->display(pl.light);
        break;
    }
    default:
        break;
    }
    return;
}

GroundStation::~GroundStation()
{
    delete ui;
}

void GroundStation::on_pushButton_6_clicked()
{

    QByteArray arr;
    float p = 6666.0f;
    arr.append(*(((char*)&p) + 0));
    arr.append(*(((char*)&p) + 1));
    arr.append(*(((char*)&p) + 2));
    arr.append(*(((char*)&p) + 3));
    arr.append((char)1);
    arr.append('\0');
    arr.append((char)2);
    arr.append('\0');
    qDebug() << link.write(3001, arr);
}
