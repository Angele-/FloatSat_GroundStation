#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QMainWindow>
#include "satellitelink.h"
#include "basics.h"
#include "imageprocessor.h"
#include "payload.h"
#include <QScrollBar>

namespace Ui {
class GroundStation;
}

class GroundStation : public QMainWindow
{
    Q_OBJECT

private slots:
    void readFromLink();

    void onSetPicRecieveStatusMaximum(qint32 maximum);

    void onSetPicRecieveStatusValue(qint32 value);

    void onSetImgSizeLbl(QString text);

    void onUpdatePicture();

    //Predefined Slots:

    void on_pushButton_Burn_clicked();

    void on_pushButton_Velocity_Mode_clicked();

    void on_pushButton_Standby_Mode_clicked();

    void on_pushButton_Position_Mode_clicked();

    void on_pushButton_RDV_Mode_clicked();

    void on_pushButton_Deployment_Mode_clicked();

    void on_pushButton_Docking_Mode_clicked();

    void on_radioButton_Motor_Clockwise_clicked();

    void on_radioButton_Motor_Counterclockwise_clicked();

    void on_spinBox_P_gain_editingFinished();

    void on_spinBox_I_gain_editingFinished();

    void on_spinBox_D_Gain_editingFinished();

    void on_pushButton_Send_clicked();

    void on_lineEdit_Command_returnPressed();

    void on_lineEdit_Satellite_returnPressed();

    void on_lineEdit_Thread_returnPressed();

    void on_lineEdit_Var_returnPressed();

    void on_lineEdit_Motor_speed_returnPressed();

public:
    explicit GroundStation(QWidget *parent = 0);
    static void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    SatelliteLink link;
    ~GroundStation();

private:
    void setPixel(Pixel p);
    void setPixelRow(PixelRow p);
    void displayImage();

    Ui::GroundStation *ui;
    static Ui::GroundStation *ui_static;

    ImageProcessor *proc;

};

#endif // GROUNDSTATION_H
