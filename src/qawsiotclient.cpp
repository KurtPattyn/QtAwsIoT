#include "qawsiotclient.h"
#include "qawsiotclient_p.h"
#include "qawsiotnetworkrequest_p.h"
#include "qmqttprotocol.h"
#include "qmqttclient.h"
#include <QString>

QAwsIoTClientPrivate::QAwsIoTClientPrivate(const QString &clientId, const QString &region,
                                           QAwsIoTClient * const q) :
    QObject(),
    q_ptr(q),
    m_mqttClient(new QMqttClient(clientId)),
    m_region(region)
{
    Q_ASSERT(q);
}

QAwsIoTClientPrivate::~QAwsIoTClientPrivate()
{}

void QAwsIoTClientPrivate::connect(const QString &hostName,
                                   const QString &accessKeyId, const QString &secretAccessKey,
                                   const QString &sessionToken)
{
    makeSignalSlotConnections();

    QAwsIoTNetworkRequest request(m_region, hostName, accessKeyId, secretAccessKey, sessionToken);

    m_mqttClient->connect(request);
}

void QAwsIoTClientPrivate::disconnect()
{
    m_mqttClient->disconnect();
}

void QAwsIoTClientPrivate::subscribe(const QString &topic, QMqttProtocol::QoS qos,
                                     std::function<void(bool)> cb)
{
    m_mqttClient->subscribe(topic, qos, cb);
}

void QAwsIoTClientPrivate::unsubscribe(const QString &topic, std::function<void (bool)> cb)
{
    m_mqttClient->unsubscribe(topic, cb);
}

void QAwsIoTClientPrivate::publish(const QString &topic, const QByteArray &message)
{
    m_mqttClient->publish(topic, message);
}

void QAwsIoTClientPrivate::publish(const QString &topic, const QByteArray &message,
                                   std::function<void (bool)> cb)
{
    m_mqttClient->publish(topic, message, cb);
}

void QAwsIoTClientPrivate::makeSignalSlotConnections()
{
    Q_Q(QAwsIoTClient);

    QObject::connect(m_mqttClient.data(), &QMqttClient::connected,
                     q, &QAwsIoTClient::connected, Qt::QueuedConnection);
    QObject::connect(m_mqttClient.data(), &QMqttClient::disconnected,
                     q, &QAwsIoTClient::disconnected, Qt::QueuedConnection);
    QObject::connect(m_mqttClient.data(), &QMqttClient::stateChanged,
                     q, &QAwsIoTClient::stateChanged, Qt::QueuedConnection);
    QObject::connect(m_mqttClient.data(), &QMqttClient::error,
                     q, &QAwsIoTClient::error, Qt::QueuedConnection);
    QObject::connect(m_mqttClient.data(), &QMqttClient::messageReceived,
                     q, &QAwsIoTClient::messageReceived, Qt::QueuedConnection);
}

QAwsIoTClient::QAwsIoTClient(const QString &clientId, const QString & region) :
    d_ptr(new QAwsIoTClientPrivate(clientId, region, this))
{
}

QAwsIoTClient::~QAwsIoTClient()
{
}

void QAwsIoTClient::connect(const QString &hostName,
                            const QString &accessKeyId, const QString &secretAccessKey,
                            const QString &sessionToken)
{
    Q_D(QAwsIoTClient);

    d->connect(hostName, accessKeyId, secretAccessKey, sessionToken);
}

void QAwsIoTClient::disconnect()
{
    Q_D(QAwsIoTClient);

    d->disconnect();
}

void QAwsIoTClient::subscribe(const QString &topic, std::function<void(bool)> cb)
{
    Q_D(QAwsIoTClient);

    d->subscribe(topic, QMqttProtocol::QoS::AT_LEAST_ONCE, cb);
}

void QAwsIoTClient::subscribe(const QString &topic, QMqttProtocol::QoS qos,
                              std::function<void(bool)> cb)
{
    Q_D(QAwsIoTClient);

    d->subscribe(topic, qos, cb);
}

void QAwsIoTClient::unsubscribe(const QString &topic, std::function<void (bool)> cb)
{
    Q_D(QAwsIoTClient);

    d->unsubscribe(topic, cb);
}

void QAwsIoTClient::publish(const QString &topic, const QByteArray &message)
{
    Q_D(QAwsIoTClient);

    d->publish(topic, message);
}

void QAwsIoTClient::publish(const QString &topic, const QByteArray &message,
                            std::function<void (bool)> cb)
{
    Q_D(QAwsIoTClient);

    d->publish(topic, message, cb);
}
