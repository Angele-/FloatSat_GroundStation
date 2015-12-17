#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QMainWindow>
#include "satellitelink.h"

namespace Ui {
class GroundStation;
}

class GroundStation : public QMainWindow
{
    Q_OBJECT
    SatelliteLink link;

private slots:
    void readFromLink();

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

public:
    explicit GroundStation(QWidget *parent = 0);
    ~GroundStation();

private:
    Ui::GroundStation *ui;
};

#endif // GROUNDSTATION_H
