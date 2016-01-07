#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QThread>

class ImageProcessor : public QThread
{
public:
    explicit ImageProcessor(QObject *parent = 0);
    void init();
    void run();
};

#endif // IMAGEPROCESSOR_H
