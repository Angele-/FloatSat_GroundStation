#ifndef UDPTEST_H
#define UDPTEST_H

#include <QUdpSocket>

class UDPTestRead : public QObject
{
    Q_OBJECT

    QUdpSocket socket;

private slots:
    void print();

public:
    explicit UDPTestRead(QObject *parent = 0);

public slots:
};

#endif // UDPTEST_H
