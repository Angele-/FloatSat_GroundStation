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
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(readSerialData()));
    connect(&link, SIGNAL(readReady()), this, SLOT(readFromLink()));
    serialInfo = new QSerialPortInfo();
    openSerialPort();
    ui->setupUi(this);
}

void GroundStation::openSerialPort(){
    /*QList<QSerialPortInfo> ports = serialInfo->availablePorts();
    QList<QSerialPortInfo>::Iterator  i;
    for(i = ports.begin(); i != ports.end(); i++){
        qDebug() << "Port: " << (*i).portName();
    }*/


    serial->setPortName("cu.FloatSat-10-SPPDev");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Connected to " << serial->portName() << endl;
    } else {
        qDebug() << "Could not connect to " << serial->portName() << endl;
    }
}

void GroundStation::readSerialData(){
    QByteArray data = serial->readAll();
    if(ui->debugConsole->toPlainText().length() > 20000) ui->debugConsole->clear();
    line.append(data);
    if(!propertiesRx && line.contains("CAMERA_TX_START;", Qt::CaseSensitive) && line.contains(";PROPS;", Qt::CaseSensitive)){
        QString props = line.mid(line.indexOf("CAMERA_TX_START;")+16,10);
        QStringList d = props.split(";",  QString::SplitBehavior::SkipEmptyParts);
        properties.Height = d.at(0).toInt();
        properties.Width = d.at(1).toInt();
        properties.type = d.at(2).toInt();
        if(properties.Height > 0 && properties.Width > 0){
            Image = cv::Mat::zeros(cv::Size(properties.Width, properties.Height), CV_8UC3);
            QString labelText = "Size: " + QString::number(properties.Height) + " x " + QString::number(properties.Width);
            ui->img_size_lbl->setText(labelText);

        }
        //qDebug() << properties.Height << " " << properties.Width << " " << properties.type;
        line = line.mid(line.indexOf(";PROPS;")+7);
        propertiesRx = true;
        sendToConsole = false;
        picFinished = false;
        //Length of a whole String containing YUV Data for a 160*120 Image including spacers
        ui->picRecieveStatus->setMaximum(properties.Width * properties.Height * 2 * 4);
        pixelCount = 0;
    }
    if(!picFinished && line.contains(";_CAMERA_TX_END;", Qt::CaseSensitive)){
        line = line.left(line.indexOf(";_CAMERA_TX_END;"));
        picFinished = true;
        propertiesRx = false;
        QStringList d = line.split(";", QString::SplitBehavior::SkipEmptyParts);

        for(QStringList::iterator i = d.begin(); i != d.end(); ++i){
            QString current = (*i);
            yuv.append(current.toInt());
        }

        ui->debugConsole->insertPlainText("\nRecieved YUV Data: " + QString::number(yuv.length()) + "\n");

        line = "";
        data.clear();
        sendToConsole = true;
        ProcessImageGray();
    }
    if(sendToConsole){
        ui->debugConsole->insertPlainText(data);
        QScrollBar *scrollbar = ui->debugConsole->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());

        if(line.length() > 1000) line = "";
    }else{
        if(propertiesRx){
            pixelCount = line.length();
            if(pixelCount < ui->picRecieveStatus->maximum()){
                ui->picRecieveStatus->setValue(pixelCount);
            }else{
                ui->picRecieveStatus->setValue(ui->picRecieveStatus->maximum());
            }
        }
    }


}

void GroundStation::ProcessImageGray(){
    int i = 1; //uYvY?

    for(int x = 0; x < Image.rows; x++){
        for(int y = 0; y < Image.cols; y++){
            if(i > yuv.size() - 1){
                Image.at<cv::Vec3b>(x,y)[0] = 0;
                Image.at<cv::Vec3b>(x,y)[1] = 0;
                Image.at<cv::Vec3b>(x,y)[2] = 0;
                //qDebug() << "Error - YUV underflow" << endl;
            }else{
                Image.at<cv::Vec3b>(x,y)[0] = (uchar) yuv.at(i);
                Image.at<cv::Vec3b>(x,y)[1] = (uchar) yuv.at(i);
                Image.at<cv::Vec3b>(x,y)[2] = (uchar) yuv.at(i);
            }
            if(properties.type == 0){
                i = i+2;
            }else{
                i++;
            }
        }
    }
    cv::transpose(Image,Image);
    cv::flip(Image,Image, 1);
    QImage image((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
    ui->picture->setPixmap(QPixmap::fromImage(image));
    yuv.clear();
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
    Telecommand command(6666.0f, 1, 2);
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

void GroundStation::on_pushButton_Calibration_clicked()
{

}

void GroundStation::on_spinBox_Motor_Speed_editingFinished()
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

    Telecommand tc(command, satellite, thread);
    link.write(3001, tc);
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

void GroundStation::on_pushButton_motor_clicked()
{
    QString str = this->findChild<QLineEdit*>("lineEdit_motor")->text();
    float command = str.toFloat();

    bool clockwise = this->findChild<QRadioButton*>("radioButton_Motor_Clockwise")->isChecked();

    if (clockwise)
        command = std::abs(command);
    else
        command = - std::abs(command);

    Telecommand tc (command, 1, 2);
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
