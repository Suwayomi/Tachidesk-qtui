#include <QQuickAsyncImageProvider>
#include <QNetworkAccessManager>

// The response object handles the actual download for a single image request.
class NetworkImageResponse : public QQuickImageResponse
{
public:
    NetworkImageResponse(const QString &url, const QString &username, const QString &password);
    QQuickTextureFactory *textureFactory() const override;

private slots:
    void onFinished();

private:
    QImage m_image;
    QNetworkReply *m_reply;
    QNetworkAccessManager *m_networkAccessManager;
};

// The provider is the factory that creates response objects.
class NetworkImageProvider : public QQuickAsyncImageProvider
{
public:
    NetworkImageProvider(const QString &username, const QString &password);
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
    QString m_username;
    QString m_password;
};
