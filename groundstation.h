#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QMainWindow>
#include "satellitelink.h"
#include "basics.h"
#include "imageprocessor.h"
#include "payload.h"
#include <QScrollBar>
#include <qcustomplot.h>

namespace Ui {
class GroundStation;
}

class GroundStation : public QMainWindow
{
    Q_OBJECT

private slots:
    void onSendPicture();

    void onSatelliteFound(int heading);

    void readFromLink();

    void onSetPicRecieveStatusMaximum(qint32 maximum);

    void onSetPicRecieveStatusValue(qint32 value);

    void onSetImgSizeLbl(QString text);

    void onUpdatePicture();

    void doPlotDataRate();

    void onSetConsoleText(QString text);

    void onSetConsoleText(QByteArray data);

    //Predefined Slots:

    void on_pushButton_Acquire_Mode_clicked();

    void on_pushButton_Docking_Mode_clicked();

    void on_lineEdit_Command_returnPressed();

    void on_lineEdit_Satellite_returnPressed();

    void on_lineEdit_Thread_returnPressed();

    void on_lineEdit_Var_returnPressed();

    void on_lineEdit_Motor_speed_returnPressed();

    void on_lineEdit_Speed_returnPressed();

    void on_lineEdit_Angle_returnPressed();

    void on_pushButton_clearConsole_clicked();

    void on_lineEdit_p_returnPressed();

    void on_lineEdit_i_returnPressed();

    void on_lineEdit_d_returnPressed();

    void on_pushButton_picture_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_stop_burn_clicked();

    void on_pushButton_calibrate_gyro_clicked();

    void on_pushButton_Deploy_Mode_clicked();

    void on_pushButton_Undeploy_Mode_clicked();

    void on_pushButton_Presentation_clicked();


    void on_checkBox_clicked(bool checked);

    void on_pushButton_burn_clicked();

public:
    explicit GroundStation(QWidget *parent = 0);
    static void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    SatelliteLink *link;
    ~GroundStation();

private:
    void setPixel(Pixel p);
    void setPixelRow(PixelRow p);
    void displayImage();
    void sendTelecommand();

    Ui::GroundStation *ui;
    static Ui::GroundStation *ui_static;

    ImageProcessor *proc;

    QCustomPlot *plotCurrent;
    QCustomPlot *plotVoltage;
    QCustomPlot *plotMotorCurrents;
    QCustomPlot *plotLight;
    QCustomPlot *plotSpeed;
    QCustomPlot *plotDataRate;
    QTimer dataRateTimer;
};

#endif // GROUNDSTATION_H
