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
    link.addTopic(PayloadCameraPropertiesType);
    link.addTopic(PayloadCameraPixelType);
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
    case PayloadCameraPropertiesType:{
        PictureProperties ps(payload);
        if(ps.Height > 0 && ps.Width > 0){
            propertiesRecieved = true;
            Image = cv::Mat::zeros(cv::Size(ps.Height, ps.Width), CV_8UC3);
            QString labelText = "Size: " + QString::number(ps.Height) + " x " + QString::number(ps.Width);
            ui->img_size_lbl->setText(labelText);
            pixelCount = rows = cols = 0;
        }
        break;
    }
    case PayloadCameraPixelType:{
        qDebug() << "PixelRow recieved" << endl;
        PixelRow pr(payload);
        if(propertiesRecieved){
            pixelCount++;
            ui->picRecieveStatus->setValue(pixelCount);
            if(pixelCount == (quint32)(Image.rows*Image.cols)){
                displayImage();
                propertiesRecieved = false;
                pixelCount = 0;
                ui->picRecieveStatus->reset();
            }
        }

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

void GroundStation::setPixel(Pixel p){
    Image.at<cv::Vec3b>(cols, rows)[0] = p.r;
    Image.at<cv::Vec3b>(cols, rows)[1] = p.g;
    Image.at<cv::Vec3b>(cols, rows)[2] = p.b;
    if(rows >= 120){
        rows = 0;
        cols++;
    }else if(rows < 120){
        rows++;
    }

}

void GroundStation::displayImage(){
    QImage image((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
    ui->picture->setPixmap(QPixmap::fromImage(image));
}
