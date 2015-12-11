#ifndef SATELLITELINK_H
#define SATELLITELINK_H

#include <QUdpSocket>
#include <QQueue>

struct SatellitePayload{
    quint16 checksum;
    quint32 senderNode;
    quint64 timestamp;
    quint32 senderThread;
    quint32 topic;
    quint16 ttl;
    quint16 userDataLen;
    quint8 userData[998];
};

class SatelliteLink : public QObject
{
    Q_OBJECT

    QHostAddress localAddress;
    QHostAddress remoteAddress;
    quint16 port;
    QUdpSocket socket;
    bool bound;
    QQueue<SatellitePayload> payloads;

signals:
    void readReady();

private slots:
    void readFromSocket();

public:
    explicit SatelliteLink(QObject *parent = 0);
    bool write(SatellitePayload payload);
    SatellitePayload read();
    bool isBound();
    bool isReadReady();
};
#endif // SATELLITELINK_H
