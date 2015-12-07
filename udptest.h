#ifndef UDPTEST_H
#define UDPTEST_H

#include <QUdpSocket>

class UDPTest : public QObject
{
    Q_OBJECT

    QUdpSocket socket;

private slots:
    void print();

public:
    explicit UDPTest(QObject *parent = 0);
    void write(QByteArray str);

public slots:
};
#endif // UDPTEST_H
