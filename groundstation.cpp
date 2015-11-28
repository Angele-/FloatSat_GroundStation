#include "groundstation.h"
#include "ui_groundstation.h"

GroundStation::GroundStation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GroundStation)
{
    ui->setupUi(this);
}

GroundStation::~GroundStation()
{
    delete ui;
}
