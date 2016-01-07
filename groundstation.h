#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QMainWindow>
#include "satellitelink.h"
#include "basics.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>

namespace Ui {
class GroundStation;
}

class GroundStation : public QMainWindow
{
    Q_OBJECT
    SatelliteLink link;

private slots:
    void readFromLink();

    void readSerialData();

    void readSerialImage();

    void on_pushButton_Burn_clicked();

    void on_pushButton_Velocity_Mode_clicked();

    void on_pushButton_Standby_Mode_clicked();

    void on_pushButton_Position_Mode_clicked();

    void on_pushButton_RDV_Mode_clicked();

    void on_pushButton_Deployment_Mode_clicked();

    void on_pushButton_Docking_Mode_clicked();

    void on_pushButton_Calibration_clicked();

    void on_spinBox_Motor_Speed_editingFinished();

    void on_radioButton_Motor_Clockwise_clicked();

    void on_radioButton_Motor_Counterclockwise_clicked();

    void on_spinBox_P_gain_editingFinished();

    void on_spinBox_I_gain_editingFinished();

    void on_spinBox_D_Gain_editingFinished();

    void on_pushButton_Send_clicked();

    void on_lineEdit_Command_returnPressed();

    void on_lineEdit_Satellite_returnPressed();

    void on_lineEdit_Thread_returnPressed();

    void on_pushButton_motor_clicked();

    void on_lineEdit_Motor_speed_returnPressed();

    void on_consoleClearBtn_clicked();

public:
    explicit GroundStation(QWidget *parent = 0);
    ~GroundStation();

private:
    void setPixel(Pixel p);
    void setPixelRow(PixelRow p);
    void displayImage();
    void openSerialPort();
    void ProcessImageGray();
    void ProcessImageV();
    Ui::GroundStation *ui;
    cv::Mat Image;
    qint32 pixelCount = 0;
    QSerialPort *serial;
    QSerialPortInfo *serialInfo;
    QString line = "";
    PictureProperties properties;
    bool picFinished = false;
    bool propertiesRx = false;
    QVector<quint8> yuv;
    bool sendToConsole = true;
    quint16 rows,cols = 0;
};

#endif // GROUNDSTATION_H
