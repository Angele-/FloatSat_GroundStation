#ifndef UDPTEST_H
#define UDPTEST_H

#include <QUdpSocket>

enum UDPMode{
    UDP_READ, UDP_WRITE
};

class UDPTest : public QObject
{
    Q_OBJECT

    UDPMode mode;
    QHostAddress localAddress;
    QHostAddress multiAddress;
    quint16 port;
    QUdpSocket socket;

private slots:
    void print();

public:
    explicit UDPTest(UDPMode mode, QObject *parent = 0);
    void executeIfWriteMode();

public slots:
};
#endif // UDPTEST_H
