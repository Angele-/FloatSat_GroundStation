#include <qmath.h>
#include <algorithm>
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
    link->addTopic(PayloadOtherSatelliteTheirType);
    connect(link, SIGNAL(readReady()), this, SLOT(readFromLink()));

    proc = new ImageProcessor(this);        //Serial Image Reader
    connect(proc, SIGNAL(updatePicture()), this, SLOT(onUpdatePicture()));
    connect(proc, SIGNAL(setImgSizeLbl(QString)), this, SLOT(onSetImgSizeLbl(QString)));
    connect(proc, SIGNAL(setPicRecieveStatusMaximum(qint32)), this, SLOT(onSetPicRecieveStatusMaximum(qint32)));
    connect(proc, SIGNAL(setPicRecieveStatusValue(qint32)), this, SLOT(onSetPicRecieveStatusValue(qint32)));
    connect(proc, SIGNAL(setConsoleText(QByteArray)), this, SLOT(onSetConsoleText(QByteArray)));
    connect(proc, SIGNAL(setConsoleText(QString)), this, SLOT(onSetConsoleText(QString)));
    connect(proc, SIGNAL(satelliteFound(int)), this, SLOT(onSatelliteFound(int)));
    connect(proc, SIGNAL(sendPicture()), this, SLOT(onSendPicture()));
    proc->init();

    plotCurrent = new QCustomPlot(this);
    ui->plotLayout->addWidget(plotCurrent, 0, 0);
    plotCurrent->addGraph(); // blue line
    plotCurrent->graph(0)->setPen(QPen(Qt::blue));
    //plotCurrent->graph(0)->setName("Battery Current");
//    plotCurrent->addGraph(); // green line
//    plotCurrent->graph(2)->setPen(QPen(Qt::darkGreen));
//    plotCurrent->graph(2)->setName("Servo 1");
//    plotCurrent->addGraph(); // black line
//    plotCurrent->graph(3)->setPen(QPen(Qt::black));
//    plotCurrent->graph(3)->setName("Servo 2");
    plotCurrent->xAxis->setLabel("Seconds");
    plotCurrent->yAxis->setLabel("Milliamperes");
    QCPPlotTitle *title = new QCPPlotTitle(plotCurrent, "Battery Current");
    title->setFont(ui->label_72->font());
    plotCurrent->plotLayout()->insertRow(0);
    plotCurrent->plotLayout()->addElement(0, 0, title);
    plotCurrent->plotLayout()->setRowSpacing(0);
    plotCurrent->plotLayout()->setColumnSpacing(0);

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
    plotMotorCurrents->graph(1)->setName("Solar Panels");
//    plotMotorCurrents->addGraph(); // red line
//    plotMotorCurrents->graph(1)->setPen(QPen(Qt::red));
//    plotMotorCurrents->graph(1)->setName("Thermal knife 1");
//    plotMotorCurrents->addGraph(); // green line
//    plotMotorCurrents->graph(2)->setPen(QPen(Qt::darkGreen));
//    plotMotorCurrents->graph(2)->setName("Thermal knife 2");
//    plotMotorCurrents->addGraph(); // black line
//    plotMotorCurrents->graph(3)->setPen(QPen(Qt::black));
//    plotMotorCurrents->graph(3)->setName("Thermal knife 3");
    plotMotorCurrents->xAxis->setLabel("Seconds");
    plotMotorCurrents->yAxis->setLabel("Milliamperes");
    title = new QCPPlotTitle(plotMotorCurrents, "Currents");
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

float static avgOfVector(const QVector<float> &vec){
    return std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
}

void GroundStation::readFromLink(){
    Payload payload = link->read();
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

        static double key = 0;
        key += PLOT_PUBLISH_INTERVAL;
        plotSpeed->graph(0)->addData(key, psg.yaw * 180.0f / M_PI);
        plotSpeed->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotSpeed->graph(0)->rescaleValueAxis(true);
        plotSpeed->graph(0)->rescaleKeyAxis();
        plotSpeed->replot();
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

        if(pm.startPictures == 1.0){
            Telecommand tc(1005, 1, 4, 0);
            link->write(3001, tc);
        }
        static QVector<float> batteryCurrents;
        static QVector<float> solarPanelCurrents;
        static QVector<float> servo1Currents;
        static QVector<float> servo2Currents;
        static QVector<float> motorACurrents;
        static QVector<float> motorBCurrents;
        static QVector<float> motorCCurrents;
        static QVector<float> motorDCurrents;

        batteryCurrents.push_back(pm.batteryCurrent);
        solarPanelCurrents.push_back(pm.panelCurrent);
        servo1Currents.push_back(pm.servo1);
        servo2Currents.push_back(pm.servo2);
        motorACurrents.push_back(pm.motorACurrent);
        motorBCurrents.push_back(pm.motorBCurrent);
        motorCCurrents.push_back(pm.motorCCurrent);
        motorDCurrents.push_back(pm.motorDCurrent);

        if(batteryCurrents.size() > 20){
            batteryCurrents.pop_front();
        }
        if(solarPanelCurrents.size() > 20){
            solarPanelCurrents.pop_front();
        }
//        if(servo1Currents.size() > 5){
//            servo1Currents.pop_front();
//        }
//        if(servo2Currents.size() > 5){
//            servo2Currents.pop_front();
//        }
        if(motorACurrents.size() > 20){
            motorACurrents.pop_front();
        }
//        if(motorBCurrents.size() > 20){
//            motorBCurrents.pop_front();
//        }
//        if(motorCCurrents.size() > 20){
//            motorCCurrents.pop_front();
//        }
//        if(motorDCurrents.size() > 20){
//            motorDCurrents.pop_front();
//        }

        ui->lcdBatteryCurrent->display(QString("%1").arg(avgOfVector(batteryCurrents), 6, 'f', 1, '0'));
        ui->lcdBatteryVoltage->display(QString("%1").arg(pm.batteryVoltage, 6, 'f', 1, '0'));
        ui->lcdPanelVoltage->display(QString("%1").arg(pm.panelVoltage, 6, 'f', 1, '0'));
        ui->lcdPanelCurrent->display(QString("%1").arg(avgOfVector(solarPanelCurrents), 6, 'f', 1, '0'));
        static double key = 0;
        key += PLOT_PUBLISH_INTERVAL;
        //plotCurrent->yAxis->setRange(0, plotCurrent->yAxis->range().upper * 0.975);
        plotCurrent->graph(0)->addData(key, avgOfVector(batteryCurrents));
        plotCurrent->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotCurrent->graph(0)->rescaleValueAxis(true);
        plotCurrent->graph(0)->rescaleKeyAxis();
//        plotCurrent->graph(2)->addData(key,avgOfVector(servo1Currents));
//        plotCurrent->graph(2)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
//        plotCurrent->graph(2)->rescaleValueAxis(true);
//        plotCurrent->graph(3)->addData(key, avgOfVector(servo2Currents));
//        plotCurrent->graph(3)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
//        plotCurrent->graph(3)->rescaleValueAxis(true);
        plotCurrent->replot();

        plotVoltage->graph(0)->addData(key, pm.batteryVoltage);
        plotVoltage->graph(1)->addData(key, pm.panelVoltage);
        plotVoltage->graph(1)->rescaleKeyAxis();
        plotVoltage->replot();

        plotMotorCurrents->yAxis->setRange(0, plotMotorCurrents->yAxis->range().upper * 0.975);
        plotMotorCurrents->graph(0)->addData(key, avgOfVector(motorACurrents));
        plotMotorCurrents->graph(0)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotMotorCurrents->graph(0)->rescaleValueAxis(true);
        plotMotorCurrents->graph(1)->addData(key, avgOfVector(solarPanelCurrents));
        plotMotorCurrents->graph(1)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
        plotMotorCurrents->graph(1)->rescaleKeyAxis();
        plotMotorCurrents->graph(1)->rescaleValueAxis(true);
//        plotMotorCurrents->graph(1)->addData(key, avgOfVector(motorBCurrents));
//        plotMotorCurrents->graph(1)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
//        plotMotorCurrents->graph(1)->rescaleValueAxis(true);
//        plotMotorCurrents->graph(2)->addData(key, avgOfVector(motorCCurrents));
//        plotMotorCurrents->graph(2)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
//        plotMotorCurrents->graph(2)->rescaleValueAxis(true);
//        plotMotorCurrents->graph(3)->addData(key, avgOfVector(motorDCurrents));
//        plotMotorCurrents->graph(3)->removeDataBefore(key - PLOT_VISIBLE_INTERVAL);
//        plotMotorCurrents->graph(3)->rescaleValueAxis(true);
//        plotMotorCurrents->graph(1)->rescaleKeyAxis();
        plotMotorCurrents->replot();

        break;
    }
    case PayloadOtherSatelliteTheirType:{
        PayloadOtherSatellite pos(payload);
        // TODO
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

void GroundStation::on_pushButton_Acquire_Mode_clicked()
{
    ui->pushButton_Acquire_Mode->setDown(true);

    Telecommand tc(0, 1, 2, 8);
    link->write(3001, tc);

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
    //command *= M_PI / 180.0f;

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
    command *= M_PI / 180.0f;

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
    ui->pushButton_picture->setDown(true);

    Telecommand tc(1005, 1, 4, 0);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_stop_clicked()
{
    ui->pushButton_Presentation->setDown(true);

    Telecommand tc(0, 1, 2, 7);
    link->write(3001, tc);

    Telecommand tc2(1002, 1, 7, 0);
    link->write(3001, tc2);
}

void GroundStation::on_pushButton_stop_burn_clicked()
{
    Telecommand tc(0, 1, 2, 9);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_calibrate_gyro_clicked()
{
    Telecommand tc(1001, 1, 1, 0);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_Deploy_Mode_clicked()
{
    ui->pushButton_Deploy_Mode->setDown(true);

    Telecommand tc(1003, 1, 5, 0);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_Undeploy_Mode_clicked()
{
    ui->pushButton_Undeploy_Mode->setDown(true);

    Telecommand tc(1002, 1, 5, 0);
    link->write(3001, tc);
}

void GroundStation::on_pushButton_Presentation_clicked()
{
    ui->pushButton_Presentation->setDown(true);

    Telecommand tc(1001, 1, 7, 0);
    link->write(3001, tc);
}

void GroundStation::onSatelliteFound(int heading){
    //get Heading
    heading *= M_PI / 180.0f;
    Telecommand tc(heading, 1, 2, 12);
    link->write(3001, tc);
}

void GroundStation::onSendPicture(){
    Telecommand tc(1005, 1, 4, 0);
    link->write(3001, tc);
}

void GroundStation::on_checkBox_clicked(bool checked)
{
    proc->cont = checked;
}
