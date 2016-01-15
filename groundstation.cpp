#include <qmath.h>

#include "groundstation.h"
#include "ui_groundstation.h"
#include <QMessageBox>
#include <QList>
#include <QScrollBar>
#include <QTextBlock>
#include <qcustomplot.h>

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


    // TEST
    QCustomPlot *plot = new QCustomPlot();
    ui->graphLayout1->addWidget(plot);
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    plot->graph(0)->setAntialiasedFill(false);
    plot->addGraph(); // red line
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(0)->setChannelFillGraph(plot->graph(1));

    plot = new QCustomPlot();
    ui->graphLayout1->addWidget(plot);
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    plot->graph(0)->setAntialiasedFill(false);
    plot->addGraph(); // red line
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(0)->setChannelFillGraph(plot->graph(1));

    plot = new QCustomPlot();
    ui->graphLayout2->addWidget(plot);
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    plot->graph(0)->setAntialiasedFill(false);
    plot->addGraph(); // red line
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(0)->setChannelFillGraph(plot->graph(1));

    plot = new QCustomPlot();
    ui->graphLayout2->addWidget(plot);
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    plot->graph(0)->setAntialiasedFill(false);
    plot->addGraph(); // red line
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(0)->setChannelFillGraph(plot->graph(1));

//    plot->addGraph(); // blue dot
//    plot->graph(2)->setPen(QPen(Qt::blue));
//    plot->graph(2)->setLineStyle(QCPGraph::lsNone);
//    plot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
//    plot->addGraph(); // red dot
//    plot->graph(3)->setPen(QPen(Qt::red));
//    plot->graph(3)->setLineStyle(QCPGraph::lsNone);
//    plot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

//    plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//    plot->xAxis->setDateTimeFormat("hh:mm:ss");
//    plot->xAxis->setAutoTickStep(false);
//    plot->xAxis->setTickStep(2);
//    plot->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    //connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //dataTimer.start(0); // Interval 0 means to refresh as fast as possible
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

void GroundStation::on_radioButton_Motor_Clockwise_clicked()
{

}

void GroundStation::on_radioButton_Motor_Counterclockwise_clicked()
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
        command = - std::abs(command);

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
