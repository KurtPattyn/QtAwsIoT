#pragma once

#include "qmqttnetworkrequest.h"
#include "qawsiot_global.h"

class QString;
class QDateTime;
//see: http://docs.aws.amazon.com/iot/latest/developerguide/protocols.html
//and http://docs.aws.amazon.com/general/latest/gr/signing_aws_api_requests.html
class QTAWSIOT_AUTOTEST_EXPORT QAwsIoTNetworkRequest: public QMqttNetworkRequest
{
public:
    QAwsIoTNetworkRequest(const QString &region, const QString &hostname,
                          const QString &accessKeyId, const QString &secretAccessKey,
                          const QString &sessionToken);
    virtual ~QAwsIoTNetworkRequest() = default;
    QAwsIoTNetworkRequest(const QAwsIoTNetworkRequest &) = default;
    //QAwsIoTNetworkRequest(QAwsIoTNetworkRequest &&) = default;

    QAwsIoTNetworkRequest &operator =(const QAwsIoTNetworkRequest &) = default;
    //QAwsIoTNetworkRequest &operator =(QAwsIoTNetworkRequest &&) = default;

private:
    void signRequest(const QString &region, const QString &hostname, const QString &serviceName,
                     const QString &accessKeyId, const QString &secretAccessKey,
                     const QString &sessionToken);
    QString createHashedCanonicalRequest(const QString &canonicalHeaders,
                                         const QString &signedHeaders) const;
    QByteArray getSignatureKey(const QString &secretAccessKey, const QString &date,
                               const QString &region,
                               const QString &serviceName) const;
    QByteArray createSignature(const QString &hashedCanonicalRequest,
                               const QString &secretAccessKey, const QString &credentialScope,
                               const QString &hashingAlgorithm,
                               const QDateTime &date,
                               const QString &region, const QString &awsServiceName) const;
};
