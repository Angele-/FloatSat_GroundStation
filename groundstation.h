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

public:
    explicit GroundStation(QWidget *parent = 0);
    ~GroundStation();

private:
    Ui::GroundStation *ui;
};

#endif // GROUNDSTATION_H
