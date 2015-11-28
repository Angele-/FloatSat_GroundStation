#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QMainWindow>

namespace Ui {
class GroundStation;
}

class GroundStation : public QMainWindow
{
    Q_OBJECT

public:
    explicit GroundStation(QWidget *parent = 0);
    ~GroundStation();

private:
    Ui::GroundStation *ui;
};

#endif // GROUNDSTATION_H
