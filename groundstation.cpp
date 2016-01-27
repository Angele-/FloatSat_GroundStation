#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"
#include <QMessageBox>
#include <QList>
#include <QScrollBar>
#include <QTextBlock>
#include <QTimer>

#define PLOT_PUBLISH_INTERVAL 0.1 // seconds
#define PLOT_VISIBLE_INTERVAL 7.5 // seconds
#define PLOT_DATA_RATE_PUBLISH_INTERVAL 0.1 // seconds
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
    link->addTopic(PayloadMeasurementsType);
    connect(link, SIGNAL(readReady()), this, SLOT(readFromLink()));

    proc = new ImageProcessor(this);        //Serial Image Reader
    connect(proc, SIGNAL(updatePicture()), this, SLOT(onUpdatePicture()));
    connect(proc, SIGNAL(setImgSizeLbl(QString)), this, SLOT(onSetImgSizeLbl(QString)));
    connect(proc, SIGNAL(setPicRecieveStatusMaximum(qint32)), this, SLOT(onSetPicRecieveStatusMaximum(qint32)));
    connect(proc, SIGNAL(setPicRecieveStatusValue(qint32)), this, SLOT(onSetPicRecieveStatusValue(qint32)));
    connect(proc, SIGNAL(setConsoleText(QByteArray)), this, SLOT(onSetConsoleText(QByteArray)));
    connect(proc, SIGNAL(setConsoleText(QString)), this, SLOT(onSetConsoleText(QString)));

    proc->init();

    plotCurrent = new QCustomPlot(this);
    ui->plotLayout->addWidget(plotCurrent, 0, 0);
    plotCurrent->addGraph(); // blue line
    plotCurrent->graph(0)->setPen(QPen(Qt::blue));
    plotCurrent->graph(0)->setName("Batteries");
    plotCurrent->addGraph(); // red line
    plotCurrent->graph(1)->setPen(QPen(Qt::red));
    plotCurrent->graph(1)->setName("Solar Panels");
    plotCurrent->addGraph(); // green line
    plotCurrent->graph(2)->setPen(QPen(Qt::green));
    plotCurrent->graph(2)->setName("Servo 1");
    plotCurrent->addGraph(); // black line
    plotCurrent->graph(3)->setPen(QPen(Qt::black));
    plotCurrent->graph(3)->setName("Servo 2");
    plotCurrent->xAxis->setLabel("Seconds");
    plotCurrent->yAxis->setLabel("Milliamperes");
    QCPPlotTitle *title = new QCPPlotTitle(plotCurrent, "Current");
    title->setFont(ui->label_72->font());
    plotCurrent->plotLayout()->insertRow(0);
    plotCurrent->plotLayout()->addElement(0, 0, title);
    plotCurrent->plotLayout()->setRowSpacing(0);
    plotCurrent->plotLayout()->setColumnSpacing(0);
    plotCurrent->legend->setVisible(true);
    plotCurrent->legend->setFont(QFont(ui->label_72->font().family(),6));
    plotCurrent->legend->setIconSize(15, 10);

    //plotCurrent->legend->rowCount()->

    plotVoltage = new QCustomPlot();
    ui->plotLayout->addWidget(plotVoltage, 0, 1);
    plotVoltage->addGraph(); // blue line
    plotVoltage->graph(0)->setPen(QPen(Qt::blue));
    plotVoltage->graph(0)->setName("Batteries");
    plotVoltage->yAxis->setRange(0, 14);
    plotVoltage->addGraph(); // red line
    plotVoltage->graph(1)->setPen(QPen(Qt::red));
    plotVoltage->graph(1)->setName("Solar Panels");
    plotVoltage->xAxis->setLabel("Seconds");
    plotVoltage->yAxis->setLabel("Volts");
    title = new QCPPlotTitle(plotVoltage, "Voltage");
    title->setFont(ui->label_72->font());
    plotVoltage->plotLayout()->insertRow(0);
    plotVoltage->plotLayout()->addElement(0, 0, title);
    plotVoltage->plotLayout()->setRowSpacing(0);
    plotVoltage->plotLayout()->setColumnSpacing(0);
    plotVoltage->legend->setVisible(true);
    plotVoltage->legend->setFont(QFont(ui->label_72->font().family(),7));
    plotVoltage->legend->setIconSize(13, 8);


    plotMotorCurrents = new QCustomPlot();
    ui->plotLayout->addWidget(plotMotorCurrents, 1, 0);
    plotMotorCurrents->addGraph(); // blue line
    plotMotorCurrents->graph(0)->setPen(QPen(Qt::blue));
    plotMotorCurrents->graph(0)->setName("Motor");
    plotMotorCurrents->addGraph(); // red line
    plotMotorCurrents->graph(1)->setPen(QPen(Qt::red));
    plotMotorCurrents->graph(1)->setName("Thermal knife 1");
    plotMotorCurrents->addGraph(); // green line
    plotMotorCurrents->graph(2)->setPen(QPen(Qt::green));
    plotMotorCurrents->graph(2)->setName("Thermal knife 2");
    plotMotorCurrents->addGraph(); // black line
    plotMotorCurrents->graph(3)->setPen(QPen(Qt::black));
    plotMotorCurrents->graph(3)->setName("Thermal knife 3");
    plotMotorCurrents->xAxis->setLabel("Seconds");
    plotMotorCurrents->yAxis->setLabel("Milliamperes");
    title = new QCPPlotTitle(plotMotorCurrents, "Motor currents");
    title->setFont(ui->label_72->font());
    plotMotorCurrents->plotLayout()->insertRow(0);
    plotMotorCurrents->plotLayout()->addElement(0, 0, title);
    plotMotorCurrents->plotLayout()->setRowSpacing(0);
    plotMotorCurrents->plotLayout()->setColumnSpacing(0);
    plotMotorCurrents->legend->setVisible(true);
    plotMotorCurrents->legend->setFont(QFont(ui->label_72->font().family(),6));
    plotMotorCurrents->legend->setIconSize(13, 8);

    plotLight = new QCustomPlot();
    ui->plotLayout->addWidget(plotLight, 1, 1);
    plotLight->yAxis->setScaleType(QCPAxis::stLogarithmic);
    plotLight->yAxis->setScaleLogBase(10);
    plotLight->yAxis->setSubTickCount(2);
    plotLight->yAxis->setRange(1, 40000);
    plotLight->addGraph(); // black line
    plotLight->graph(0)->setPen(QPen(Qt::black));
    plotLight->xAxis->setLabel("Seconds");
    plotLight->yAxis->setLabel(" ");
    title = new QCPPlotTitle(plotLight, "Light");
    title->setFont(ui->label_72->font());
    plotLight->plotLayout()->insertRow(0);
    plotLight->plotLayout()->addElement(0, 0, title);
    plotLight->plotLayout()->setRowSpacing(0);
    plotLight->plotLayout()->setColumnSpacing(0);

    plotSpeed = new QCustomPlot();
    ui->plotLayout->addWidget(plotSpeed, 2, 0);
    plotSpeed->addGraph(); // black line
    plotSpeed->graph(0)->setPen(QPen(Qt::black));
    plotSpeed->xAxis->setLabel("Seconds");
    plotSpeed->yAxis->setLabel("Radians / second");
    title = new QCPPlotTitle(plotSpeed, "Speed");
    title->setFont(ui->label_72->font());
    plotSpeed->plotLayout()->insertRow(0);
    plotSpeed->plotLayout()->addElement(0, 0, title);
    plotSpeed->plotLayout()->setRowSpacing(0);
    plotSpeed->plotLayout()->setColumnSpacing(0);

    plotDataRate = new QCustomPlot();
    ui->plotLayout->addWidget(plotDataRate, 2, 1);
    plotDataRate->addGraph(); // blue line
    plotDataRate->graph(0)->setPen(QPen(Qt::blue));
    plotDataRate->graph(0)->setName("Bluetooth");
    plotDataRate->addGraph(); // red line
    plotDataRate->graph(1)->setPen(QPen(Qt::red));
    plotDataRate->graph(1)->setName("WiFi");
    plotDataRate->xAxis->setLabel("Seconds");
    plotDataRate->yAxis->setLabel("Bytes / Second");
    title = new QCPPlotTitle(plotDataRate, "Data rate");
    title->setFont(ui->label_72->font());
    plotDataRate->plotLayout()->insertRow(0);
    plotDataRate->plotLayout()->addElement(0, 0, title);
    plotDataRate->plotLayout()->setRowSpacing(0);
    plotDataRate->plotLayout()->setColumnSpacing(0);
    plotDataRate->legend->setVisible(true);
    plotDataRate->legend->setFont(QFont(ui->label_72->font().family(),7));
    plotDataRate->legend->setIconSize(15, 10);

    connect(&dataRateTimer, SIGNAL(timeout()), this, SLOT(doPlotDataRate()));
    dataRateTimer.start(PLOT_DATA_RATE_PUBLISH_INTERVAL * 1000);

}

void GroundStation::doPlotDataRate(){
    static double key = 0;
    key += PLOT_DATA_RATE_PUBLISH_INTERVAL;
    plotDataRate->yAxis->setRange(0, plotDataRate->yAxis->range().upper * 0.975);
    plotDataRate->graph(0)->addData(key, (proc->readAndResetReceivedBytes()) * 1.0 / PLOT_DATA_RATE_PUBLISH_INTERVAL);
    plotDataRate->graph(0)->removeDataBefore(key - PLOT_DATA_RATE_VISIBLE_INTERVAL);
    plotDataRate->graph(0)->rescaleValueAxis(true);
    plotDataRate->graph(1)->addData(key, (link->readAndResetReceivedBytes() + link->readAndResetSentBytes())* 1.0 / PLOT_DATA_RATE_PUBLISH_INTERVAL);
    plotDataRate->graph(1)->removeDataBefore(key - PLOT_DATA_RATE_VISIBLE_INTERVAL);
    plotDataRate->graph(1)->rescaleKeyAxis();
    plotDataRate->graph(1)->rescaleValueAxis(true);
    plotDataRate->replot();
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

    str.replace("\"", "");
    str.replace("\\t", "\t");
    str.replace("\\r", "\r");
    str.replace("\\n", "\n");
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
        plotLight->graph(0)->addData(key, pl.light);
        plotLight->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotLight->graph(0)->rescaleKeyAxis();
        plotLight->replot();
        break;
    }
    case PayloadMeasurementsType:{
        PayloadMeasurements pm(payload);
        ui->lcdBatteryCurrent->display(QString("%1").arg(pm.batteryCurrent, 6, 'f', 1, '0'));
        ui->lcdBatteryVoltage->display(QString("%1").arg(pm.batteryVoltage/2000.0f, 6, 'f', 1, '0'));
        ui->lcdPanelVoltage->display(QString("%1").arg(pm.panelVoltage, 6, 'f', 1, '0'));
        ui->lcdPanelCurrent->display(QString("%1").arg(pm.panelCurrent, 6, 'f', 1, '0'));

        static double key = 0;
        key += PLOT_PUBLISH_INTERVAL;
        plotCurrent->yAxis->setRange(0, plotCurrent->yAxis->range().upper * 0.975);
        plotCurrent->graph(0)->addData(key, pm.batteryCurrent);
        plotCurrent->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotCurrent->graph(0)->rescaleValueAxis(true);
        plotCurrent->graph(1)->addData(key, pm.panelCurrent);
        plotCurrent->graph(1)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotCurrent->graph(1)->rescaleKeyAxis();
        plotCurrent->graph(1)->rescaleValueAxis(true);
        plotCurrent->graph(2)->addData(key, pm.servo1);
        plotCurrent->graph(2)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotCurrent->graph(2)->rescaleValueAxis(true);
        plotCurrent->graph(3)->addData(key, pm.servo2);
        plotCurrent->graph(3)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotCurrent->graph(3)->rescaleValueAxis(true);
        plotCurrent->replot();

        plotVoltage->graph(0)->addData(key, pm.batteryVoltage/2000.0f);
        plotVoltage->graph(1)->addData(key, pm.panelVoltage);
        plotVoltage->graph(1)->rescaleKeyAxis();
        plotVoltage->replot();

        plotMotorCurrents->graph(0)->addData(key, pm.motorACurrent);
        plotMotorCurrents->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotMotorCurrents->graph(0)->rescaleValueAxis(true);
        plotMotorCurrents->graph(1)->addData(key, pm.motorBCurrent);
        plotMotorCurrents->graph(1)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotMotorCurrents->graph(1)->rescaleValueAxis(true);
        plotMotorCurrents->graph(2)->addData(key, pm.motorCCurrent);
        plotMotorCurrents->graph(2)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotMotorCurrents->graph(2)->rescaleValueAxis(true);
        plotMotorCurrents->graph(3)->addData(key, pm.motorDCurrent);
        plotMotorCurrents->graph(3)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotMotorCurrents->graph(3)->rescaleValueAxis(true);



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

void GroundStation::on_pushButton_Acquire_Mode_clicked()
{
    ui->pushButton_Acquire_Mode->setDown(true);

    Telecommand tc(0, 1, 2, 8);
    link->write(3001, tc);

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

void GroundStation::on_lineEdit_p_returnPressed()
{
    QString str = ui->lineEdit_p->text();
    float command = str.toFloat();

    Telecommand tc(command, 1, 2, 1);
    link->write(3001, tc);
}

void GroundStation::on_lineEdit_i_returnPressed()
{
    QString str = ui->lineEdit_i->text();
    float command = str.toFloat();

    Telecommand tc(command, 1, 2, 3);
    link->write(3001, tc);
}

void GroundStation::on_lineEdit_d_returnPressed()
{
    QString str = ui->lineEdit_d->text();
    float command = str.toFloat();

    Telecommand tc(command, 1, 2, 2);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_Automatic_clicked()
{
    ui->pushButton_Automatic->setDown(true);
}

void GroundStation::onSetConsoleText(QString text){
    ui->debugConsole->insertPlainText(text);
    QScrollBar* scrollbar = ui->debugConsole->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    if(ui->debugConsole->toPlainText().length() > 20000) ui->debugConsole->clear();
}

void GroundStation::onSetConsoleText(QByteArray data){
    ui->debugConsole->insertPlainText(data);
    QScrollBar* scrollbar = ui->debugConsole->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    if(ui->debugConsole->toPlainText().length() > 20000) ui->debugConsole->clear();
}

void GroundStation::on_pushButton_picture_clicked()
{
    Telecommand tc(1005, 1, 4, 0);
    link->write(3001, tc);
}
