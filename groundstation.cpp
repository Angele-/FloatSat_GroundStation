#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"
#include <QMessageBox>
#include <QList>
#include <QScrollBar>

GroundStation::GroundStation(QWidget *parent) :
    QMainWindow(parent), link(parent), ui(new Ui::GroundStation)
{
    link.addTopic(PayloadSensorFusionType);
    link.addTopic(PayloadSensorGyroType);
    link.addTopic(PayloadSensorXMType);
    link.addTopic(PayloadLightType);
    link.addTopic(PayloadCounterType);
    link.addTopic(PayloadCameraPropertiesType);
    link.addTopic(PayloadCameraPixelType);
    connect(&link, SIGNAL(readReady()), this, SLOT(readFromLink()));

    proc = new ImageProcessor(this);        //Serial Image Reader
    connect(proc, SIGNAL(updatePicture()), this, SLOT(onUpdatePicture()));
    connect(proc, SIGNAL(setConsoleText(QByteArray)), this, SLOT(onSetConsoleText(QByteArray)));
    connect(proc, SIGNAL(setConsoleText(QString)), this, SLOT(onSetConsoleText(QString)));
    connect(proc, SIGNAL(setImgSizeLbl(QString)), this, SLOT(onSetImgSizeLbl(QString)));
    connect(proc, SIGNAL(setPicRecieveStatusMaximum(qint32)), this, SLOT(onSetPicRecieveStatusMaximum(qint32)));
    connect(proc, SIGNAL(setPicRecieveStatusValue(qint32)), this, SLOT(onSetPicRecieveStatusValue(qint32)));
    proc->init();

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
    case PayloadLightType:{
        PayloadLight pl(payload);
        this->findChild<QLCDNumber*>("lcdNumber_32")->display(pl.light);
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
    default:
        break;
    }
    return;
}

GroundStation::~GroundStation()
{
    delete ui;
}

void GroundStation::on_pushButton_Burn_clicked()
{
    Telecommand command(666.0f, 1, 2, 6);
    link.write(3001, command);
}

void GroundStation::on_pushButton_Velocity_Mode_clicked()
{

}

void GroundStation::on_pushButton_Standby_Mode_clicked()
{

}

void GroundStation::on_pushButton_Position_Mode_clicked()
{

}

void GroundStation::on_pushButton_RDV_Mode_clicked()
{

}

void GroundStation::on_pushButton_Deployment_Mode_clicked()
{

}

void GroundStation::on_pushButton_Docking_Mode_clicked()
{

}

void GroundStation::on_radioButton_Motor_Clockwise_clicked()
{

}

void GroundStation::on_radioButton_Motor_Counterclockwise_clicked()
{

}

void GroundStation::on_spinBox_P_gain_editingFinished()
{

}

void GroundStation::on_spinBox_I_gain_editingFinished()
{

}

void GroundStation::on_spinBox_D_Gain_editingFinished()
{

}

void GroundStation::on_pushButton_Send_clicked()
{
    QString str = this->findChild<QLineEdit*>("lineEdit_Command")->text();
    float command = str.toFloat();

    str = this->findChild<QLineEdit*>("lineEdit_Satellite")->text();
    quint16 satellite = str.toUShort();

    str = this->findChild<QLineEdit*>("lineEdit_Thread")->text();
    quint16 thread = str.toUShort();

    str = this->findChild<QLineEdit*>("lineEdit_Var")->text();
    quint32 variableId = str.toUShort();

    Telecommand tc(command, satellite, thread, variableId);
    link.write(3001, tc);

    if(command == 1003.0 && thread == 4 && satellite == 1){
        proc->setTransmissionFinished(true);
        qDebug() << "Transmission finished";
    }
}


void GroundStation::on_lineEdit_Command_returnPressed()
{
    on_pushButton_Send_clicked();
}

void GroundStation::on_lineEdit_Satellite_returnPressed()
{
    on_pushButton_Send_clicked();
}

void GroundStation::on_lineEdit_Thread_returnPressed()
{
    on_pushButton_Send_clicked();
}

void GroundStation::on_lineEdit_Var_returnPressed()
{
    on_pushButton_Send_clicked();
}

void GroundStation::on_pushButton_motor_clicked()
{
    QString str = this->findChild<QLineEdit*>("lineEdit_motor")->text();
    float command = str.toFloat();

    bool clockwise = this->findChild<QRadioButton*>("radioButton_Motor_Clockwise")->isChecked();

    if (clockwise)
        command = std::abs(command);
    else
        command = - std::abs(command);

    Telecommand tc(command, 1, 2, 7);
    link.write(3001, tc);
}

void GroundStation::on_lineEdit_Motor_speed_returnPressed()
{
    on_pushButton_motor_clicked();
}

void GroundStation::on_consoleClearBtn_clicked()
{
    ui->debugConsole->clear();
}

void GroundStation::onSetPicRecieveStatusMaximum(qint32 maximum){
    ui->picRecieveStatus->setMaximum(maximum);
}

void GroundStation::onSetPicRecieveStatusValue(qint32 value){
    ui->picRecieveStatus->setValue(value);
}

void GroundStation::onSetConsoleText(QString text){
    ui->debugConsole->insertPlainText(text);
    QScrollBar* scrollbar = ui->debugConsole->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    if(ui->debugConsole->toPlainText().length() > 20000) ui->debugConsole->clear();
}

void GroundStation::onSetConsoleText(QByteArray data){
    ui->debugConsole->insertPlainText(data);
    QScrollBar* scrollbar = ui->debugConsole->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    if(ui->debugConsole->toPlainText().length() > 20000) ui->debugConsole->clear();
}

void GroundStation::onSetImgSizeLbl(QString text){
    ui->img_size_lbl->setText(text);
}

void GroundStation::onUpdatePicture(){
    ui->picture->setPixmap(QPixmap::fromImage(proc->getImage()));
}
