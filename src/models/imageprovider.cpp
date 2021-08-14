#include <QString>
#include <QUrl>
#include <QGuiApplication>
#include <QtGlobal>
#include <QDebug>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QDateTime>
#include "imageprovider.h"

const int ImageProvider::MSEC_PER_DOWNLOAD = 16; // ~ 256kbit/sec for 2k images

ImageProvider::ImageProvider(const QString imageProviderName, const QString extension, const QString cacheDirName) : QObject(),
    _cacheProvider(this), _imageProviderName(imageProviderName), _extension(extension) {

    activeDownloadCount = 0;

    _bulkDownloadTimer.setInterval(MSEC_PER_DOWNLOAD);
    _bulkDownloadTimer.setSingleShot(true);
    connect(&_bulkDownloadTimer, &QTimer::timeout, this, &ImageProvider::bulkDownloadStep);

    QString useCacheDirName = cacheDirName != "" ? cacheDirName : imageProviderName;
    _cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QString("/" + useCacheDirName);
}

ImageProvider::~ImageProvider() {
}

/*********************************************
 *
 * method: makeAvailable
 *
 ********************************************/
bool ImageProvider::makeAvailable(QString key) {
    key = getCanonicalKey(key);

    if (currentlyDownloading.contains(key)) {
        // download of this emote in progress
        return true;
    }
    else if (download(key)) {
        // if this emote isn't already downloading, it's safe to load the cache file or download if not in the cache
        currentlyDownloading.insert(key);
        activeDownloadCount += 1;
        return true;
    }
    else {
        // we already had the emote locally and don't need to wait for it to download
        return false;
    }
}

/*********************************************
 *
 * method: download
 *
 ********************************************/
bool ImageProvider::download(QString key) {
    if (_imageTable.contains(key)) {
        return false;
    }

    QString name = key;
    QString ext = _extension;
    if(key.contains(".")) {
      auto keyList = key.split('.');
      name = keyList.front();
      ext = "." + keyList.back();
    }

    const QUrl url = getUrlForKey(name);
    _cacheDir.mkpath(".");

    QString filename = _cacheDir.absoluteFilePath(name + ext);

    if (_cacheDir.exists(name + ext)) {
        loadImageFile(name, filename);
        return false;
    }

    QNetworkRequest request(url);
    QNetworkReply* _reply = nullptr;
    _reply = _manager.get(request);

    DownloadHandler * dh = new DownloadHandler(filename, name + ext);

    connect(_reply, &QNetworkReply::readyRead,
        dh, &DownloadHandler::dataAvailable);
    connect(_reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
        dh, &DownloadHandler::error);
    connect(_reply, &QNetworkReply::finished,
        dh, &DownloadHandler::replyFinished);
    connect(dh, &DownloadHandler::downloadComplete,
        this, &ImageProvider::individualDownloadComplete);

    return true;
}

void ImageProvider::bulkDownloadStep() {
    for (auto& key : _curBulkDownloadKeys)
    {
      if (makeAvailable(key)) {
          // hit us back when the next time interval is up
          _bulkDownloadTimer.start();
          return;
      }
      else {
          qApp->processEvents();
      }
    }

    emit bulkDownloadComplete();
}

void ImageProvider::bulkDownload(const QList<QString> & keys) {
    _curBulkDownloadKeys = keys;

    bulkDownloadStep();
}


void ImageProvider::individualDownloadComplete(QString filename, bool hadError) {
    DownloadHandler * dh = qobject_cast<DownloadHandler*>(sender());
    const QString emoteKey = dh->getKey();
    auto fixedKey = emoteKey.left(emoteKey.indexOf("."));
    delete dh;

    if (hadError) {
        // delete partial download if any
        QFile(filename).remove();
    }
    else {
        loadImageFile(fixedKey, filename);
    }

    if (activeDownloadCount > 0) {
        activeDownloadCount--;
    }

    currentlyDownloading.remove(fixedKey);

    if (activeDownloadCount == 0) {
        emit downloadComplete();
    }
}

QHash<QString, QImage> ImageProvider::imageTable() {
    return _imageTable;
}

void ImageProvider::loadImageFile(QString emoteKey, QString filename) {
    QImage emoteImg;
    emoteImg.load(filename);
    _imageTable.insert(emoteKey, emoteImg);
}

QQmlImageProviderBase * ImageProvider::getQMLImageProvider() {
    return &_cacheProvider;
}

bool ImageProvider::downloadsInProgress() const {
    return activeDownloadCount > 0;
}


URLFormatImageProvider::URLFormatImageProvider(const QString imageProviderName, const QString urlFormat, const QString extension, const QString cacheDir) :
    ImageProvider(imageProviderName, extension, cacheDir), _urlFormat(urlFormat)
{

}

const QUrl URLFormatImageProvider::getUrlForKey(QString & key) {
    return _urlFormat.arg(key);
}

QString ImageProvider::getCanonicalKey(const QString key) {
    return key;
}

// DownloadHandler

DownloadHandler::DownloadHandler(QString filename, QString key) : filename(filename), key(key), hadError(false) {
    _file.setFileName(filename);
    _file.open(QFile::WriteOnly);
    qDebug() << "save to" << filename;
}

void DownloadHandler::dataAvailable() {
    QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
    auto buffer = _reply->readAll();
    _file.write(buffer.data(), buffer.size());
}

void DownloadHandler::error(QNetworkReply::NetworkError /*code*/) {
    hadError = true;
    QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
}

void DownloadHandler::replyFinished() {
    QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
    if (_reply) {
        _reply->deleteLater();
        _file.commit();
        //might need something for windows for the forwardslash..

        emit downloadComplete(_file.fileName(), hadError);
    }
}


// CachedImageProvider
CachedImageProvider::CachedImageProvider(ImageProvider const* provider) : QQuickImageProvider(QQuickImageProvider::Image), _provider(provider) {

}

QImage CachedImageProvider::requestImage(const QString &id, QSize * size, const QSize & /*requestedSize*/)
{
    // TODO figure out something sensible to do re requestedSize
    qDebug() << "Requested id" << id << "from image provider";

    auto result = _provider->_imageTable.find(id);
    if (result != _provider->_imageTable.end()) {
        *size = result.value().size();
        return result.value();
    }
    return QImage();
}
