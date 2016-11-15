#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include <QProcessEnvironment>
#include "qawsiotclient.h"
#include "qmqttprotocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!env.contains(QStringLiteral("AWS_ACCESS_KEY_ID")) ||
            !env.contains(QStringLiteral("AWS_SECRET_ACCESS_KEY")))
    {
        qFatal("Please set the AWS_ACCESS_KEY_ID and AWS_SECRET_ACCESS_KEY environment variables.");
    }

    const QString region = "eu-west-1";

    //the following four parameters are received via a 'getAccessTokens' call
    const QString hostname = "data.iot." + region + ".amazonaws.com";
    const QString accessKeyId = env.value(QStringLiteral("AWS_ACCESS_KEY_ID"));
    const QString secretAccessKey = env.value(QStringLiteral("AWS_SECRET_ACCESS_KEY"));
    const QString sessionToken;
    const QString clientId = "meMyselfAndI";

    QAwsIoTClient client(clientId, region);
    client.connect(&client, &QAwsIoTClient::connected, [&client]() {
        qDebug() << "Connected to AWS IoT";
        client.subscribe("resources/wsid1234/messages/in", [&](bool result) {
            qDebug() << "Subscription result" << result;
            client.publish("resources/wsid1234/messages/in", "Some message", [](bool result) {
                qDebug() << "Published message; succeeded =" << result;
            });
        });
    });
    client.connect(&client, &QAwsIoTClient::disconnected, []() {
        qDebug() << "Disconnected from AWS IoT";
    });
    client.connect(&client, &QAwsIoTClient::error, [](QMqttProtocol::Error err, QString errorMessage) {
        qDebug() << "Error" << err << errorMessage;
    });
    client.connect(&client, &QAwsIoTClient::messageReceived,
                   [](const QString &topicName, const QByteArray &message) {
        qDebug() << "Received message on topic" << topicName << ":" << message;
    });

    client.connect(hostname, accessKeyId, secretAccessKey, sessionToken);

    return a.exec();
}
