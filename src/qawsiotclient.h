#pragma once

#include "qmqttprotocol.h"
#include "qawsiot_global.h"
#include <QScopedPointer>

class QAwsIoTClientPrivate;
class QByteArray;
class QString;
class QTAWSIOT_EXPORT QAwsIoTClient: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QAwsIoTClient)
    Q_DECLARE_PRIVATE(QAwsIoTClient)

public:
    QAwsIoTClient(const QString &clientId, const QString &region);
    virtual ~QAwsIoTClient();

    QString region() const;

    using QObject::connect;
    void connect(const QString &hostName, const QString &accessKeyId,
                 const QString &secretAccessKey, const QString &sessionToken = QString());
    using QObject::disconnect;
    void disconnect();

    void subscribe(const QString &topic, std::function<void(bool)> cb);
    void subscribe(const QString &topic, QMqttProtocol::QoS qos, std::function<void(bool)> cb);
    void unsubscribe(const QString &topic, std::function<void(bool)> cb);
    /**
     * @brief Publishes `message` to the given `topic` with a QoS of 0.
     * The `topic` should not be empty and should not contain wildcard characters, otherwise
     * the server will drop the connection.
     * The `message` can be empty.
     */
    void publish(const QString &topic, const QByteArray &message);
    /**
     * @brief Publishes `message` to the given `topic` with a QoS of 1. When acknowledgement of
     * the server is received the given callback is called.
     * The `topic` should not be empty and should not contain wildcard characters, otherwise
     * the server will drop the connection.
     * The `message` can be empty.
     */
    void publish(const QString &topic, const QByteArray &message, std::function<void(bool)> cb);

Q_SIGNALS:
    void stateChanged(QMqttProtocol::State);
    void connected();
    void disconnected();
    void error(QMqttProtocol::Error error, const QString &errorMessage);
    void messageReceived(const QString &topicName, const QByteArray &message);

private:
    QScopedPointer<QAwsIoTClientPrivate> d_ptr;
};
