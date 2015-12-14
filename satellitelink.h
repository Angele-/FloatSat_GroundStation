#ifndef SATELLITELINK_H
#define SATELLITELINK_H

#include <QUdpSocket>
#include <QQueue>
#include <QSet>
#include "payload.h"

class SatelliteLink : public QObject
{
    Q_OBJECT

    QHostAddress localAddress;
    QHostAddress remoteAddress;
    quint16 port;
    QUdpSocket socket;
    bool bound;
    bool checkChecksum;
    QSet<quint32> topics;
    QQueue<PayloadSatellite> payloads;

signals:
    void readReady();

private slots:
    void readFromSocket();

public:
    explicit SatelliteLink(QObject *parent = 0, bool checkChecksum = false);
    void addTopic(PayloadType);
    int write(quint32 topicId, QByteArray &data);
    PayloadSatellite read();
    bool isBound();
    bool isReadReady();
};
#endif // SATELLITELINK_H
