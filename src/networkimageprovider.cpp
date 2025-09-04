#include "networkimageprovider.h"
#include <QNetworkRequest>
#include <QImageReader>
#include <qnetworkreply.h>
#include <qquickimageprovider.h>

// --- NetworkImageResponse Implementation ---
NetworkImageResponse::NetworkImageResponse(const QString &url, const QString &username, const QString &password)
{
    // Create QNetworkAccessManager in the current thread
    m_networkAccessManager = new QNetworkAccessManager(this);
    
    QNetworkRequest request(url);
    
    // Add Basic Authentication header if credentials are provided
    if (!username.isEmpty() && !password.isEmpty()) {
        QString credentials = QString("%1:%2").arg(username, password);
        QByteArray encodedCredentials = credentials.toUtf8().toBase64();
        request.setRawHeader("Authorization", "Basic " + encodedCredentials);
    }

    m_reply = m_networkAccessManager->get(request);
    connect(m_reply, &QNetworkReply::finished, this, &NetworkImageResponse::onFinished);
}

void NetworkImageResponse::onFinished()
{
    if (m_reply->error() == QNetworkReply::NoError) {
        QImageReader reader(m_reply);
        if (reader.canRead()) {
            m_image = reader.read();
        } else {
            qWarning() << "Could not read image data for" << m_reply->url();
        }
    } else {
        qWarning() << "Network error:" << m_reply->errorString();
    }
    m_reply->deleteLater();
    m_reply = nullptr;

    emit finished(); // IMPORTANT: Signal that the download and processing are complete.
}

QQuickTextureFactory *NetworkImageResponse::textureFactory() const
{
    // Return a texture factory for the downloaded image.
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}


// --- NetworkImageProvider Implementation ---
NetworkImageProvider::NetworkImageProvider(const QString &username, const QString &password)
    : m_username(username), m_password(password) {}

QQuickImageResponse *NetworkImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    // 'id' will be the URL passed from QML.
    // We ignore requestedSize for simplicity here, but you could use it.
    Q_UNUSED(requestedSize);
    return new NetworkImageResponse(id, m_username, m_password);
}

