#include "qawsiotnetworkrequest_p.h"

#include <QString>
#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QVector>
#include <numeric>      //for std::accumulate
#include <algorithm>    //for std::sort

QString vectorToDelimitedString(const QVector<QString> &vector, const QString &delimiter)
{
    const QString result =
            vector.empty() ?
                QString() :
                std::accumulate(vector.begin() + 1, vector.end(), vector.front(),
                                [&](QString const& s, const QString &value) { return s + delimiter + value; });

    return result;
}

inline QString getDateTimeString(const QDateTime &date)
{
    return date.toString("yyyyMMddThhmmssZ");
}

inline QString getDateString(const QDateTime &date)
{
    return date.toString("yyyyMMdd");
}

QAwsIoTNetworkRequest::QAwsIoTNetworkRequest(const QString &region, const QString &hostname,
                                             const QString &accessKeyId,
                                             const QString &secretAccessKey,
                                             const QString &sessionToken,
                                             qint64 timestamp) :
    QMqttNetworkRequest()
{
    signRequest(region, hostname, "iotdata", accessKeyId, secretAccessKey, sessionToken, timestamp);
}

void QAwsIoTNetworkRequest::signRequest(const QString &region,
                                        const QString &hostname,
                                        const QString &serviceName,
                                        const QString &accessKeyId,
                                        const QString &secretAccessKey,
                                        const QString &sessionToken,
                                        qint64 timestamp)
{
    QVector<QString> headerNamesToEncode = { "host", "X-Amz-Date" };
    const QString hashingAlgorithm = "AWS4-HMAC-SHA256";
    const QDateTime current = QDateTime::fromMSecsSinceEpoch(timestamp).toUTC();
    //const QDateTime current = QDateTime::currentDateTimeUtc();

    const QString &credentialScope =
            getDateString(current) + "/" + region + "/" + serviceName +"/aws4_request";

    QUrl url;
    url.setHost(hostname);
    url.setScheme("wss");
    url.setPort(443);
    url.setPath("/mqtt");
    setUrl(url);
    setRawHeader(QByteArrayLiteral("X-Amz-Date"), getDateTimeString(current).toUtf8());

    QVector<QString> canonicalHeaderList;

    //when calculating the signature, headers must be sorted ascending
    //so make sure they are sorted
    std::sort(headerNamesToEncode.begin(), headerNamesToEncode.end(),
              [](const QString &s1, const QString &s2) { return s1.toLower() < s2.toLower(); });
    for (const QString &headerName : headerNamesToEncode) {
        if (headerName.toLower() == "host") {
            //must add port to host, as this is what QWebSocket does by default
            canonicalHeaderList << QStringLiteral("host:%1:%2")
                                   .arg(url.host(QUrl::FullyEncoded)).arg(url.port(443));
        } else {
            const QString value = QString::fromUtf8(rawHeader(headerName.toUtf8()));
            canonicalHeaderList << headerName.toLower() + ":" + value;
        }
    }

    const QString canonicalHeaders = vectorToDelimitedString(canonicalHeaderList, "\n") + "\n";

    QVector<QString> lowerCaseHeaderNames = headerNamesToEncode;
    std::for_each(lowerCaseHeaderNames.begin(),
                  lowerCaseHeaderNames.end(),
                  [](QString &s) { s = s.toLower(); });
    const QString signedHeaders = vectorToDelimitedString(lowerCaseHeaderNames, ";");

    const QString hashedCanonicalRequest
            = createHashedCanonicalRequest(canonicalHeaders, signedHeaders);
    const QByteArray signature = createSignature(hashedCanonicalRequest,
                                                 secretAccessKey, credentialScope,
                                                 hashingAlgorithm,
                                                 current, region, serviceName);

    //Step 4: Add signing information to the request
    //see: http://docs.aws.amazon.com/general/latest/gr/sigv4-add-signature-to-request.html
    /* Header format:
       Authorization: algorithm Credential=access key ID/credential scope, SignedHeaders=SignedHeaders, Signature=signature

       Example header:
       Authorization: AWS4-HMAC-SHA256 Credential=AKIDEXAMPLE/20150830/us-east-1/iam/aws4_request, SignedHeaders=content-type;host;x-amz-date, Signature=5d672d79c15b13162d9279b0855cfba6789a8edb4c82c400e06b5924a6f2b5d7
    */
    const QString authorizationHeader = QStringLiteral("%1 Credential=%2, SignedHeaders=%3, Signature=%4")
            .arg(hashingAlgorithm)
            .arg(accessKeyId + "/" + credentialScope)
            .arg(signedHeaders)
            .arg(QString::fromUtf8(signature.toHex()));

    //setting the appropriate headers
    setRawHeader(QByteArrayLiteral("Authorization"), authorizationHeader.toUtf8());

    if (!sessionToken.isEmpty()) {
        setRawHeader(QByteArrayLiteral("X-Amz-Security-Token"), sessionToken.toUtf8());
    }
}

QByteArray QAwsIoTNetworkRequest::getSignatureKey(const QString &secretAccessKey,
                                                  const QString &date, const QString &region,
                                                  const QString &serviceName) const
{
    const QByteArray kDate =
            QMessageAuthenticationCode::hash(date.toUtf8(), ("AWS4" + secretAccessKey).toUtf8(),
                                             QCryptographicHash::Sha256);
    const QByteArray kRegion =
            QMessageAuthenticationCode::hash(region.toUtf8(), kDate,
                                             QCryptographicHash::Sha256);
    const QByteArray kService =
            QMessageAuthenticationCode::hash(serviceName.toUtf8(), kRegion,
                                             QCryptographicHash::Sha256);
    const QByteArray kSigning =
            QMessageAuthenticationCode::hash(QByteArrayLiteral("aws4_request"), kService,
                                             QCryptographicHash::Sha256);
    return kSigning;
}

QByteArray QAwsIoTNetworkRequest::createSignature(const QString &hashedCanonicalRequest,
                                                  const QString &secretAccessKey,
                                                  const QString &credentialScope,
                                                  const QString &hashingAlgorithm,
                                                  const QDateTime &date,
                                                  const QString &region,
                                                  const QString &awsServiceName) const
{
    const QString dateString = getDateString(date);
    const QString dateTimeString = getDateTimeString(date);

    //Step 2: Create a string to sign for SigV4
    //see: http://docs.aws.amazon.com/general/latest/gr/sigv4-create-string-to-sign.html
    const QString stringToSign = hashingAlgorithm + "\n" +
            dateTimeString + "\n" +
            credentialScope + "\n" +
            hashedCanonicalRequest;

    const QByteArray signingKey = getSignatureKey(secretAccessKey, dateString,
                                                  region, awsServiceName);

    //Step 3: Create the signature
    //see: http://docs.aws.amazon.com/general/latest/gr/sigv4-calculate-signature.html
    const QByteArray signature = QMessageAuthenticationCode::hash(stringToSign.toUtf8(), signingKey,
                                                                  QCryptographicHash::Sha256);
    return signature;
}

QString QAwsIoTNetworkRequest::createHashedCanonicalRequest(const QString &canonicalHeaders,
                                                            const QString &signedHeaders) const
{
    //Step 1: create canonical request
    //see: http://docs.aws.amazon.com/general/latest/gr/sigv4-create-canonical-request.html
    const QString httpRequestMethod = "GET";
    const QString canonicalURI = url().path(QUrl::FullyEncoded);
    const QString payload;  //there is no payload with an opening handshake request;
    //however it must be hashed

    const QString canonicalQueryString = url().query(QUrl::FullyEncoded);

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(payload.toUtf8());
    const QString hashedPayload = QString::fromUtf8(hash.result().toHex()).toLower();

    QVector<QString> canonicalRequestItems;
    canonicalRequestItems << httpRequestMethod
                          << canonicalURI
                          << canonicalQueryString
                          << canonicalHeaders
                          << signedHeaders
                          << hashedPayload;
    const QString canonicalRequest = vectorToDelimitedString(canonicalRequestItems, "\n");

    hash.reset();
    hash.addData(canonicalRequest.toUtf8());
    return QString::fromUtf8(hash.result().toHex()).toLower();
}
