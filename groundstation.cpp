#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"
#include <QMessageBox>
#include <QList>
#include <QScrollBar>
#include <QTextBlock>
#include <QTimer>

#define PLOT_PUBLISH_INTERVAL 0.01 // seconds
#define PLOT_VISIBLE_INTERVAL 7.5 // seconds
#define PLOT_DATA_RATE_PUBLISH_INTERVAL 0.2 // seconds
#define PLOT_DATA_RATE_VISIBLE_INTERVAL 15 // seconds

Ui::GroundStation *GroundStation::ui_static = NULL;

GroundStation::GroundStation(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::GroundStation)
{
    ui_static = ui;
    ui->setupUi(this);

    link = new SatelliteLink(this);
    link->addTopic(PayloadSensorFusionType);
    link->addTopic(PayloadSensorGyroType);
    link->addTopic(PayloadSensorXMType);
    link->addTopic(PayloadLightType);
    link->addTopic(PayloadCounterType);
    link->addTopic(PayloadCameraPropertiesType);
    link->addTopic(PayloadCameraPixelType);
    connect(link, SIGNAL(readReady()), this, SLOT(readFromLink()));

    proc = new ImageProcessor(this);        //Serial Image Reader
    connect(proc, SIGNAL(updatePicture()), this, SLOT(onUpdatePicture()));
    connect(proc, SIGNAL(setImgSizeLbl(QString)), this, SLOT(onSetImgSizeLbl(QString)));
    connect(proc, SIGNAL(setPicRecieveStatusMaximum(qint32)), this, SLOT(onSetPicRecieveStatusMaximum(qint32)));
    connect(proc, SIGNAL(setPicRecieveStatusValue(qint32)), this, SLOT(onSetPicRecieveStatusValue(qint32)));
    proc->init();

    plotCurrent = new QCustomPlot(this);
    ui->plotLayout->addWidget(plotCurrent, 0, 0);
    plotCurrent->addGraph(); // blue line
    plotCurrent->graph(0)->setPen(QPen(Qt::blue));
    plotCurrent->graph(0)->setAntialiasedFill(false);
    plotCurrent->addGraph(); // red line
    plotCurrent->graph(1)->setPen(QPen(Qt::red));
    plotCurrent->graph(1)->setAntialiasedFill(false);

    plotVoltage = new QCustomPlot();
    ui->plotLayout->addWidget(plotVoltage, 0, 1);
    plotVoltage->addGraph(); // blue line
    plotVoltage->graph(0)->setPen(QPen(Qt::blue));
    plotVoltage->graph(0)->setAntialiasedFill(false);
    plotVoltage->addGraph(); // red line
    plotVoltage->graph(1)->setPen(QPen(Qt::red));
    plotVoltage->graph(1)->setAntialiasedFill(false);

    plotPWM = new QCustomPlot();
    ui->plotLayout->addWidget(plotPWM, 1, 0);
    plotPWM->addGraph(); // black line
    plotPWM->graph(0)->setPen(QPen(Qt::black));
    plotPWM->graph(0)->setAntialiasedFill(false);

    plotLight = new QCustomPlot();
    ui->plotLayout->addWidget(plotLight, 1, 1);
    plotLight->addGraph(); // black line
    plotLight->graph(0)->setPen(QPen(Qt::black));
    plotLight->graph(0)->setAntialiasedFill(false);

    plotSpeed = new QCustomPlot();
    ui->plotLayout->addWidget(plotSpeed, 2, 0);
    plotSpeed->addGraph(); // black line
    plotSpeed->graph(0)->setPen(QPen(Qt::black));
    plotSpeed->graph(0)->setAntialiasedFill(false);

    plotDataRate = new QCustomPlot();
    ui->plotLayout->addWidget(plotDataRate, 2, 1);
    plotDataRate->addGraph(); // black line
    plotDataRate->graph(0)->setPen(QPen(Qt::black));
    plotDataRate->graph(0)->setAntialiasedFill(false);

    connect(&dataRateTimer, SIGNAL(timeout()), this, SLOT(doPlotDataRate));
    dataRateTimer.start(PLOT_DATA_RATE_PUBLISH_INTERVAL * 1000);

    QTimer *dataTimer = new QTimer();
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0);
}

void GroundStation::doPlotDataRate(){
    static double key = 0;
    key += PLOT_DATA_RATE_PUBLISH_INTERVAL;
    plotSpeed->graph(0)->addData(key, link->readAndResetReceivedBytes() + proc->readAndResetReceivedBytes());
    plotSpeed->graph(0)->removeDataBefore(key - PLOT_DATA_RATE_VISIBLE_INTERVAL);
    plotSpeed->graph(0)->rescaleAxes();
    plotSpeed->replot();
}

void GroundStation::realtimeDataSlot(){
    static double lastPointKey = 0;
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    if (key - lastPointKey > 0.010){
        double value = qSin(key);
        // add data to lines:
        plotSpeed->graph(0)->addData(key, value);
        // remove data of lines that's outside visible range:
        plotSpeed->graph(0)->removeDataBefore(key-6.29);
        // rescale value (vertical) axis to fit the current data:
        plotSpeed->graph(0)->rescaleAxes();
        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    //ui->customPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    plotSpeed->replot();
}

void GroundStation::logHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg){
    QString str;
    if(context.line){
        str += "FROM: ";
        str += context.file;
        str += ":";
        str += QString::number(context.line);
        str += ", ";
        str += context.function;
        str += "\n";
    }
    str += msg;

    QPlainTextEdit *console = ui_static->logConsole;
    if(type == QtDebugMsg){
        console = ui_static->debugConsole;
    }

    console->appendPlainText(str);
    if(console->document()->blockCount() > 2000){
        QTextBlock block = console->document()->begin();
        QTextCursor cursor(block);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }
    console->verticalScrollBar()->setValue(console->verticalScrollBar()->maximum());
}

void GroundStation::readFromLink(){
    PayloadSatellite payload = link->read();
    switch(payload.topic){
    case PayloadSensorFusionType:{
        PayloadSensorFusion psd(payload);
        ui->lcdNumber_6->display(QString("%1").arg(psd.roll * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->lcdNumber_8->display(QString("%1").arg(psd.pitch * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->lcdNumber_10->display(QString("%1").arg(psd.yaw * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->compass_widget->heading = psd.yaw * 180.0f / M_PI;
        break;
    }
    case PayloadSensorGyroType:{
        PayloadSensorGyro psg(payload);
        ui->lcdNumber_29->display(QString("%1").arg(psg.roll * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->lcdNumber_30->display(QString("%1").arg(psg.pitch * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->lcdNumber_31->display(QString("%1").arg(psg.yaw * 180.0f / M_PI, 6, 'f', 1, '0'));
        break;
    }
    case PayloadSensorXMType:{
        PayloadSensorXM psx(payload);
        ui->lcdNumber_5->display(QString("%1").arg(psx.roll * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->lcdNumber_7->display(QString("%1").arg(psx.pitch * 180.0f / M_PI, 6, 'f', 1, '0'));
        ui->lcdNumber_9->display(QString("%1").arg(psx.yaw * 180.0f / M_PI, 6, 'f', 1, '0'));
        break;
    }
    case PayloadLightType:{
        PayloadLight pl(payload);
        ui->lcdLight->display(pl.light);

        static double key = 0;
        key += PLOT_PUBLISH_INTERVAL;
        plotSpeed->graph(0)->addData(key, pl.light);
        plotSpeed->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotSpeed->graph(0)->rescaleAxes();
        plotSpeed->replot();
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
    link->write(3001, command);
}

void GroundStation::on_pushButton_Velocity_Mode_clicked()
{
    ui->pushButton_Velocity_Mode->setDown(true);
}

void GroundStation::on_pushButton_Standby_Mode_clicked()
{
    ui->pushButton_Standby_Mode->setDown(true);
}

void GroundStation::on_pushButton_Position_Mode_clicked()
{
    ui->pushButton_Position_Mode->setDown(true);
}

void GroundStation::on_pushButton_RDV_Mode_clicked()
{
    ui->pushButton_RDV_Mode->setDown(true);
}

void GroundStation::on_pushButton_Deployment_Mode_clicked()
{
    ui->pushButton_Deployment_Mode->setDown(true);
}

void GroundStation::on_pushButton_Docking_Mode_clicked()
{
    ui->pushButton_Docking_Mode->setDown(true);
}

void GroundStation::sendTelecommand()
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
    link->write(3001, tc);

    if(command == 1003.0 && thread == 4 && satellite == 1){
        proc->setTransmissionFinished(true);
        qDebug() << "Transmission finished";
    }
}


void GroundStation::on_lineEdit_Command_returnPressed()
{
    sendTelecommand();
}

void GroundStation::on_lineEdit_Satellite_returnPressed()
{
    sendTelecommand();
}

void GroundStation::on_lineEdit_Thread_returnPressed()
{
    sendTelecommand();
}

void GroundStation::on_lineEdit_Var_returnPressed()
{
    sendTelecommand();
}

void GroundStation::on_lineEdit_Motor_speed_returnPressed()
{
    QString str = ui->lineEdit_Motor_speed->text();
    float command = str.toFloat();

    bool clockwise = ui->radioButton_Motor_Clockwise->isChecked();

    if (clockwise)
        command = std::abs(command);
    else
        command = -std::abs(command);

    Telecommand tc(command, 1, 2, 7);
    link->write(3001, tc);
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

void GroundStation::on_lineEdit_Speed_returnPressed()
{
    QString str = ui->lineEdit_Speed->text();
    float command = str.toFloat();

    Telecommand tc(command, 1, 2, 5);
    link->write(3001, tc);
}

void GroundStation::on_lineEdit_Angle_returnPressed()
{
    QString str = ui->lineEdit_Angle->text();
    float command = str.toFloat();
    command *= M_PI / 180.0f;

    Telecommand tc(command, 1, 2, 4);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_clearConsole_clicked()
{
    ui->debugConsole->clear();
    ui->logConsole->clear();
}
