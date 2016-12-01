#pragma once

#include <QObject>
#include <QString>
#include <QScopedPointer>
#include "qmqttprotocol.h"
#include "qawsiotclient.h"

class QMqttClient;
class QAwsIoTClientPrivate: public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(QAwsIoTClientPrivate)
    Q_DECLARE_PUBLIC(QAwsIoTClient)

public:
    QAwsIoTClientPrivate(const QString &clientId,
                         const QString &willTopic, const QByteArray &willMessage,
                         QAwsIoTClient * const q);
    QAwsIoTClientPrivate(const QString &clientId, QAwsIoTClient * const q);
    virtual ~QAwsIoTClientPrivate();

    void connect(const QString &hostName, const QString &region, const QString &accessKeyId,
                 const QString &secretAccessKey, const QString &sessionToken);
    void disconnect();
    void subscribe(const QString &topic, QMqttProtocol::QoS qos, std::function<void(bool)> cb);
    void unsubscribe(const QString &topic, std::function<void (bool)> cb);
    void publish(const QString &topic, const QByteArray &message);
    void publish(const QString &topic, const QByteArray &message, std::function<void(bool)> cb);

private:
    QAwsIoTClient * const q_ptr;

    QScopedPointer<QMqttClient> m_mqttClient;

private:
    void makeSignalSlotConnections();
};
