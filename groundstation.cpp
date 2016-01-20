#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"
#include <QMessageBox>
#include <QList>
#include <QScrollBar>
#include <QTextBlock>
#include <QHBoxLayout>
#include <qcustomplot.h>
#include <QTimer>

static QCustomPlot *globalPlot;
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

    QCustomPlot *plot = new QCustomPlot(this);
    globalPlot = plot;
    ui->graphLayout1->addWidget(plot);
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setAntialiasedFill(false);
    plot->addGraph(); // red line
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(1)->setAntialiasedFill(false);

    plot = new QCustomPlot();
    ui->graphLayout1->addWidget(plot);
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setAntialiasedFill(false);
    plot->addGraph(); // red line
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(1)->setAntialiasedFill(false);

    QHBoxLayout *graphLayout2 = new QHBoxLayout();
    ui->graphLayout1->addLayout(graphLayout2);

    plot = new QCustomPlot();
    graphLayout2->addWidget(plot);
    plot->addGraph(); // black line
    plot->graph(0)->setPen(QPen(Qt::black));
    plot->graph(0)->setAntialiasedFill(false);

    plot = new QCustomPlot();
    graphLayout2->addWidget(plot);
    plot->addGraph(); // black line
    plot->graph(0)->setPen(QPen(Qt::black));
    plot->graph(0)->setAntialiasedFill(false);

    QTimer *dataTimer = new QTimer();
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0);
}

void GroundStation::realtimeDataSlot(){
    static double lastPointKey = 0;
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    if (key - lastPointKey > 0.010){
        double value = qSin(key);
        // add data to lines:
        globalPlot->graph(0)->addData(key, value);
        // remove data of lines that's outside visible range:
        globalPlot->graph(0)->removeDataBefore(key-6.29);
        // rescale value (vertical) axis to fit the current data:
        globalPlot->graph(0)->rescaleAxes();
        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    //ui->customPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    globalPlot->replot();
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
    str += "\n";

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
        ui->lcdNumber_6->display(QString("%1").arg(psd.roll * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->lcdNumber_8->display(QString("%1").arg(psd.pitch * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->lcdNumber_10->display(QString("%1").arg(psd.yaw * 180.0f / M_PI, 5, 'g', 2, '0'));
        break;
    }
    case PayloadSensorGyroType:{
        PayloadSensorGyro psg(payload);
        ui->lcdNumber_29->display(QString("%1").arg(psg.roll * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->lcdNumber_30->display(QString("%1").arg(psg.pitch * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->lcdNumber_31->display(QString("%1").arg(psg.yaw * 180.0f / M_PI, 5, 'g', 2, '0'));
        break;
    }
    case PayloadSensorXMType:{
        PayloadSensorXM psx(payload);

        ui->lcdNumber_5->display(QString("%1").arg(psx.roll * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->lcdNumber_7->display(QString("%1").arg(psx.pitch * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->lcdNumber_9->display(QString("%1").arg(psx.yaw * 180.0f / M_PI, 5, 'g', 2, '0'));
        ui->compass_widget->heading = psx.yaw * 180.0f / M_PI;

        break;
    }
    case PayloadLightType:{
        PayloadLight pl(payload);
        ui->lcdNumber_32->display(pl.light);
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

}

void GroundStation::on_pushButton_Standby_Mode_clicked()
{

}

void GroundStation::on_pushButton_Position_Mode_clicked()
{

}

void GroundStation::on_pushButton_RDV_Mode_clicked()
{

}

void GroundStation::on_pushButton_Deployment_Mode_clicked()
{

}

void GroundStation::on_pushButton_Docking_Mode_clicked()
{

}

void GroundStation::on_spinBox_P_gain_editingFinished()
{

}

void GroundStation::on_spinBox_I_gain_editingFinished()
{

}

void GroundStation::on_spinBox_D_Gain_editingFinished()
{

}

void GroundStation::on_pushButton_Send_clicked()
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
    on_pushButton_Send_clicked();
}

void GroundStation::on_lineEdit_Satellite_returnPressed()
{
    on_pushButton_Send_clicked();
}

void GroundStation::on_lineEdit_Thread_returnPressed()
{
    on_pushButton_Send_clicked();
}

void GroundStation::on_lineEdit_Var_returnPressed()
{
    on_pushButton_Send_clicked();
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
