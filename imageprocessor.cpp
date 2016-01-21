#include "imageprocessor.h"
#include <QDebug>

ImageProcessor::ImageProcessor(QObject *parent) :
    QThread(parent)
{
    serial = new QSerialPort(this);
    serialInfo = new QSerialPortInfo();
}

void ImageProcessor::init(){
    openSerialPort();
    connect(serial, SIGNAL(readyRead()), this, SLOT(readSerialImage()));
}


void ImageProcessor::run(){

}

void ImageProcessor::openSerialPort(){
    QList<QSerialPortInfo> ports = serialInfo->availablePorts();
    QList<QSerialPortInfo>::Iterator  i;
    for(i = ports.begin(); i != ports.end(); i++){
        qDebug() << "Port: " << (*i).portName();
    }
    #ifdef Q_OS_WIN
        serial->setPortName("COM7");
    #elif Q_OS_MAC
        serial->setPortName("cu.FloatSat-10-SPPDev");
    #endif
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Connected to " << serial->portName() << endl;
    } else {
        qDebug() << "Could not connect to " << serial->portName() << endl;
    }
}

void ImageProcessor::readSerialData(){
    QByteArray data = serial->readAll();
    line.append(data);
    if(!propertiesRx && line.contains("CAMERA_TX_START;", Qt::CaseSensitive) && line.contains(";PROPS;", Qt::CaseSensitive)){
        QString props = line.mid(line.indexOf("CAMERA_TX_START;")+16,10);
        QStringList d = props.split(";",  QString::SplitBehavior::SkipEmptyParts);
        properties.Height = d.at(0).toInt();
        properties.Width = d.at(1).toInt();
        properties.type = d.at(2).toInt();
        imagesize = properties.Width * properties.Height;
        if(properties.Height > 0 && properties.Width > 0){
            Image = cv::Mat::zeros(cv::Size(properties.Width, properties.Height), CV_8UC3);
            QString labelText = "Size: " + QString::number(properties.Height) + " x " + QString::number(properties.Width);
            emit setImgSizeLbl(labelText);

        }
        qDebug() << properties.Height << " " << properties.Width << " " << properties.type;
        line = line.mid(line.indexOf(";PROPS;")+7);
        propertiesRx = true;
        sendToConsole = false;
        picFinished = false;
        //Length of a whole String containing YUV Data for a 160*120 Image including spacers
        emit setPicRecieveStatusMaximum(properties.Width * properties.Height);
        pixelCount = 0;
    }
    if(!picFinished && line.contains(";_CAMERA_TX_END;", Qt::CaseSensitive)){
        line = line.left(line.indexOf(";_CAMERA_TX_END;"));
        picFinished = true;
        propertiesRx = false;
        QStringList d = line.split(";", QString::SplitBehavior::SkipEmptyParts);

        for(QStringList::iterator i = d.begin(); i != d.end(); ++i){
            QString current = (*i);
            yuv.append(current.toInt());
        }

        qDebug() << "\nReceived YUV Data: " << yuv.length();

        line = "";
        data.clear();
        sendToConsole = true;
        ProcessImageGray();
    }
    if(sendToConsole){
        qDebug() << data;

        if(line.length() > 1000) line = "";
    }else{
        if(propertiesRx){
            pixelCount = line.length();
            if(pixelCount < imagesize){
                picRecieveStatusValue ++;
                emit setPicRecieveStatusValue(picRecieveStatusValue);
            }else{
                emit setPicRecieveStatusValue(imagesize);
            }
        }
    }
}

void ImageProcessor::readSerialImage(){
    QByteArray data = serial->readAll();
    line.append(data);
    if(!propertiesRx && line.contains("CAMERA_TX_START;", Qt::CaseSensitive) && line.contains(";PROPS;", Qt::CaseSensitive)){
        QString props = line.mid(line.indexOf("CAMERA_TX_START;")+16,10);
        QStringList d = props.split(";",  QString::SplitBehavior::SkipEmptyParts);
        properties.Height = d.at(0).toInt();
        properties.Width = d.at(1).toInt();
        imagesize = properties.Width * properties.Height;
        properties.type = d.at(2).toInt();
        if(properties.Height > 0 && properties.Width > 0){
            Image = cv::Mat::zeros(cv::Size(properties.Width, properties.Height), CV_8UC3);
            QString labelText = "Size: " + QString::number(properties.Height) + " x " + QString::number(properties.Width);
            emit setImgSizeLbl(labelText);

        }
        line = line.mid(line.indexOf(";PROPS;")+7);
        propertiesRx = true;
        sendToConsole = false;
        picFinished = false;
        //Length of a whole String containing YUV Data for a 160*120 Image including spacers
        emit setPicRecieveStatusMaximum(properties.Width * properties.Height);
        picRecieveStatusValue = 0;
        emit setPicRecieveStatusValue(picRecieveStatusValue);
        pixelCount = 0;
    }

    if(!sendToConsole){
        //End Condition

        if(transmissionFinished && line.contains(";")){
            //Fill Image with Zeros and reset Everything
            for(quint16 x = rows; x < Image.rows; x++){
                for(quint16 y = cols; y < Image.cols; y++){
                    Image.at<cv::Vec3b>(x, y)[0] = 0;
                    Image.at<cv::Vec3b>(x, y)[1] = 0;
                    Image.at<cv::Vec3b>(x, y)[2] = 0;
                }
            }
            DetectCircles(Image);
            cv::transpose(Image,Image);
            cv::flip(Image,Image, 1);
            imageToDisplay = QImage((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
            emit updatePicture();
            rows = cols = 0;
            picRecieveStatusValue = 0;
            emit setPicRecieveStatusValue(picRecieveStatusValue);
            Image = cv::Mat::zeros(cv::Size(properties.Width, properties.Height), CV_8UC3);
            line = "";

            sendToConsole = true;
            transmissionFinished = false;
            qDebug() << "Console reenabled\n";
            propertiesRx = false;
            return;
        }
        if(line.contains(";")){
            //Fill Image with Zeros and reset Everything
            for(quint16 x = rows; x < Image.rows; x++){
                for(quint16 y = cols; y < Image.cols; y++){
                    Image.at<cv::Vec3b>(x, y)[0] = 0;
                    Image.at<cv::Vec3b>(x, y)[1] = 0;
                    Image.at<cv::Vec3b>(x, y)[2] = 0;
                }
            }
            DetectCircles(Image);
            cv::transpose(Image,Image);
            cv::flip(Image,Image, 1);
            imageToDisplay = QImage((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
            emit updatePicture();
            rows = cols = 0;
            picRecieveStatusValue = 0;
            emit setPicRecieveStatusValue(picRecieveStatusValue);
            Image = cv::Mat::zeros(cv::Size(properties.Width, properties.Height), CV_8UC3);
            line = "";
            return;
        }
        //As long as theres data available, get 1 Pixel and put it into the picture
        while(line.length() > 2){
            QString pxl = line.left(3);
            line = line.mid(3);
            picRecieveStatusValue ++;
            emit setPicRecieveStatusValue(picRecieveStatusValue);
            Image.at<cv::Vec3b>(rows, cols)[0] = pxl.toInt();
            Image.at<cv::Vec3b>(rows, cols)[1] = pxl.toInt();
            Image.at<cv::Vec3b>(rows, cols)[2] = pxl.toInt();

            if(rows == Image.rows - 1 && cols == Image.cols - 1){
                DetectCircles(Image);
                cv::transpose(Image,Image);
                cv::flip(Image,Image, 1);
                imageToDisplay = QImage((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
                emit updatePicture();
                rows = cols = 0;
                picRecieveStatusValue = 0;
                emit setPicRecieveStatusValue(picRecieveStatusValue);
                line = "";
                break;
            }else if(cols == Image.cols-1){
                cols = 0;
                rows++;
            }else{
                cols++;
            }

        }
    }else{
       qDebug() << data;
    }

}

void ImageProcessor::ProcessImageV(){
    int i = 0; //vYuY
    bool even = true;
    for(int x = 0; x < Image.rows; x++){
        for(int y = 0; y < Image.cols; y++){
            if(i > yuv.size() - 1){
                Image.at<cv::Vec3b>(x,y)[0] = 0;
                Image.at<cv::Vec3b>(x,y)[1] = 0;
                Image.at<cv::Vec3b>(x,y)[2] = 0;
                qDebug() << "Error - YUV underflow" << endl;
            }else{
                Image.at<cv::Vec3b>(x,y)[0] = (uchar) yuv.at(i);
                Image.at<cv::Vec3b>(x,y)[1] = (uchar) yuv.at(i);
                Image.at<cv::Vec3b>(x,y)[2] = (uchar) yuv.at(i);
            }
            if(even){
                even = false;
            }else{
                even = true;
                i = i + 4;
            }

        }
    }
    cv::transpose(Image,Image);
    cv::flip(Image,Image, 1);
    imageToDisplay = QImage((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
    emit updatePicture();

    yuv.clear();
}

void ImageProcessor::ProcessImageGray(){
    int i = 1; //vYuY

    for(int x = 0; x < Image.rows; x++){
        for(int y = 0; y < Image.cols; y++){
            if(i > yuv.size() - 1){
                Image.at<cv::Vec3b>(x,y)[0] = 0;
                Image.at<cv::Vec3b>(x,y)[1] = 0;
                Image.at<cv::Vec3b>(x,y)[2] = 0;
                qDebug() << "Error - YUV underflow" << endl;
            }else{
                Image.at<cv::Vec3b>(x,y)[0] = (uchar) yuv.at(i);
                Image.at<cv::Vec3b>(x,y)[1] = (uchar) yuv.at(i);
                Image.at<cv::Vec3b>(x,y)[2] = (uchar) yuv.at(i);
            }
            if(properties.type == 0){
                i = i+2;
            }else{
                i++;
            }
        }
    }
    cv::transpose(Image,Image);
    cv::flip(Image,Image, 1);
    imageToDisplay = QImage((uchar*)Image.data, Image.cols, Image.rows, Image.step, QImage::Format_RGB888);
    emit updatePicture();

    yuv.clear();
}

void ImageProcessor::setTransmissionFinished(bool t){
    transmissionFinished = t;
}

QImage ImageProcessor::getImage(){
    return imageToDisplay;
}

cv::Mat ImageProcessor::getMat(){
    return Image;
}

void ImageProcessor::DetectCircles(cv::Mat src){
    cv::Mat src_gray;
    cv::cvtColor( src, src_gray, CV_BGR2GRAY );

    cv::GaussianBlur( src_gray, src_gray, cv::Size(1, 1), 2, 2 );

    std::vector<cv::Vec3f> circles;

    cv::HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, 10, 80, 40, 0, 0);

    for(size_t i = 0; i < circles.size(); i++){
        if(circles[i][2] > 15.0 && circles[i][2] < 20.0){
            cv::Point center(circles[i][0], circles[i][1]);
            cv::circle( src, center, circles[i][2], cv::Scalar(0,255,0), 3, 8, 0 );
            qDebug() << "Center (x,y): " << circles[i][0] << circles[i][1] << " R: " << circles[i][2];
        }
    }

}
