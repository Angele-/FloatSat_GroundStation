#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QThread>
#include "basics.h"
#include "payload.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include <QImage>

class ImageProcessor : public QThread
{
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = 0);
    void init();
    void run();
    void setTransmissionFinished(bool t);
    QImage getImage();
    cv::Mat getMat();

private:
    void openSerialPort();
    void ProcessImageGray();
    void ProcessImageV();
    void DetectCircles(cv::Mat src);

    cv::Mat Image;
    quint32 pixelCount = 0;
    QSerialPort *serial;
    QSerialPortInfo *serialInfo;
    QString line = "";
    PictureProperties properties;
    quint32 imagesize;
    bool picFinished = false;
    bool propertiesRx = false;
    QVector<quint8> yuv;
    bool sendToConsole = true;
    quint16 rows,cols = 0;
    bool transmissionFinished = false;
    QImage imageToDisplay;
    quint32 picRecieveStatusValue = 0;
signals:
    void setPicRecieveStatusMaximum(qint32 maximum);
    void setPicRecieveStatusValue(qint32 value);
    void setConsoleText(QString text);
    void setConsoleText(QByteArray data);
    void setImgSizeLbl(QString text);
    void updatePicture();
private slots:
    void readSerialData();
    void readSerialImage();

};

#endif // IMAGEPROCESSOR_H