#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"
#include <QMessageBox>
#include <QList>
#include <QScrollBar>

Ui::GroundStation *GroundStation::ui_static = NULL;

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
    connect(proc, SIGNAL(setImgSizeLbl(QString)), this, SLOT(onSetImgSizeLbl(QString)));
    connect(proc, SIGNAL(setPicRecieveStatusMaximum(qint32)), this, SLOT(onSetPicRecieveStatusMaximum(qint32)));
    connect(proc, SIGNAL(setPicRecieveStatusValue(qint32)), this, SLOT(onSetPicRecieveStatusValue(qint32)));
    proc->init();

    ui_static = ui;
    ui->setupUi(this);
}

void GroundStation::logHandler(QtMsgType type, const QMessageLogContext&, const QString &msg){
    QString logString;
    switch (type) {
    case QtDebugMsg:
        logString = "Debug: ";
        break;
    case QtInfoMsg:
        logString = "Info: ";
        break;
    case QtWarningMsg:
        logString = "Warning: ";
        break;
    case QtCriticalMsg:
        logString = "Critical: ";
        break;
    case QtFatalMsg:
        logString = "Fatal: ";
    }
    logString += msg;
//    logString += " (";
//    logString += context.file;
//    logString += ":";
//    logString += QString::number(context.line);
//    logString += ", ";
//    logString += context.function;
//    logString += ")\n";
    logString += "\n";
    ui_static->logConsole->appendPlainText(logString);
    ui_static->logConsole->verticalScrollBar()->setValue(ui_static->logConsole->verticalScrollBar()->maximum());
    if(type == QtFatalMsg)
        abort();
}

void GroundStation::readFromLink(){
    PayloadSatellite payload = link.read();
    switch(payload.topic){
    case PayloadSensorFusionType:{
        PayloadSensorFusion psd(payload);
        ui->lcdNumber_6->display(psd.roll * 180.0f / M_PI);
        ui->lcdNumber_8->display(psd.pitch * 180.0f / M_PI);
        ui->lcdNumber_10->display(psd.yaw * 180.0f / M_PI);
        break;
    }
    case PayloadSensorGyroType:{
        PayloadSensorGyro psg(payload);
        ui->lcdNumber_29->display(psg.roll * 180.0f / M_PI);
        ui->lcdNumber_30->display(psg.pitch * 180.0f / M_PI);
        ui->lcdNumber_31->display(psg.yaw * 180.0f / M_PI);
        break;
    }
    case PayloadSensorXMType:{
        PayloadSensorXM psx(payload);

        ui->lcdNumber_5->display(psx.roll * 180.0f / M_PI);
        ui->lcdNumber_7->display(psx.pitch * 180.0f / M_PI);
        ui->lcdNumber_9->display(psx.yaw * 180.0f / M_PI);
        ui->compass_widget->heading = psx.yaw * 180.0f / M_PI;

        break;
    }
    case PayloadLightType:{
        PayloadLight pl(payload);
        ui->lcdNumber_32->display(pl.light);
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
    QString str = ui->lineEdit_Command->text();
    float command = str.toFloat();

    str = ui->lineEdit_Satellite->text();
    quint16 satellite = str.toUShort();

    str = ui->lineEdit_Thread->text();
    quint16 thread = str.toUShort();

    str = ui->lineEdit_Var->text();
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
    QString str = ui->lineEdit_Motor_speed->text();
    float command = str.toFloat();

    bool clockwise = ui->radioButton_Motor_Clockwise->isChecked();

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

void GroundStation::onSetPicRecieveStatusMaximum(qint32 maximum){
    ui->picRecieveStatus->setMaximum(maximum);
}

void GroundStation::onSetPicRecieveStatusValue(qint32 value){
    ui->picRecieveStatus->setValue(value);
}

void GroundStation::onSetImgSizeLbl(QString text){
    ui->img_size_lbl->setText(text);
}

void GroundStation::onUpdatePicture(){
    ui->picture->setPixmap(QPixmap::fromImage(proc->getImage()));
}

void GroundStation::on_logClearButton_clicked()
{
    ui->logConsole->clear();
}
