#include "assetdownloader.h"

#include <QtGlobal>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QCryptographicHash>

// TODO: Update to optionally also download the extra.qrb if present
AssetDownloader::AssetDownloader(const QStringList &fileName, QObject *parent )
    : QObject( parent )
    , m_fileNames( fileName )
{
}

static const QCryptographicHash::Algorithm HASH_ALGORITHM = QCryptographicHash::Sha1;
static const int HASH_BIT_SIZE = 160;
Q_STATIC_ASSERT_X( HASH_BIT_SIZE % 8 == 0 , "Invalid hash bit size" );

static QByteArray calculateAssetFileHash( const QString &fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
        return QByteArray();

    QCryptographicHash hasher( HASH_ALGORITHM );
    hasher.addData( &file );

    return hasher.result();
}

static QByteArray downloadAssetFileHash( const QUrl &hashUrl )
{
    QNetworkAccessManager manager;
    QEventLoop eventLoop;

    QNetworkRequest request( hashUrl );
    QNetworkReply *reply = manager.get( request );

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot( true );
    timeoutTimer.start( 10000 );

    QObject::connect( reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit );
    QObject::connect( &timeoutTimer, &QTimer::timeout, &eventLoop, &QEventLoop::quit );

    eventLoop.exec();

    if ( !reply->isFinished() ) // timed out
        return QByteArray();

    if ( reply->error() != QNetworkReply::NoError )
        return QByteArray();

    QByteArray result = reply->readAll();

    if ( result.size() < ( HASH_BIT_SIZE / 4 + 2 ) ) // bytes for the hex-encoded hash, + 2 spaces, + the filename
        return QByteArray();

    return QByteArray::fromHex( result.left( HASH_BIT_SIZE / 4 ) );
}

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader( const QUrl &downloadPath, const QString &localFileName, QObject *parent = nullptr )
        : QObject( parent )
        , m_file( localFileName )
        , m_reply( nullptr )
        , m_finished( false )
        , m_hasError( false )
    {
        if ( !m_file.open( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
            QMetaObject::invokeMethod(this, "onOpenFileError", Qt::QueuedConnection);
            return;
        }

        QNetworkRequest request( downloadPath );
        m_reply = m_manager.get( request );
        connect( m_reply, &QNetworkReply::readyRead, this, &FileDownloader::onReplyReadyRead );
        connect( m_reply, &QNetworkReply::finished, this, &FileDownloader::onReplyFinished );
    }

    bool isFinished() const { return m_finished; }
    bool hasError() const { return m_hasError; }

signals:
    void finished();

private slots:
    void onReplyReadyRead()
    {
        m_file.write( m_reply->readAll() );

        // TODO: reset a timeout timer?
    }

    void onReplyFinished()
    {
        if ( m_reply->error() != QNetworkReply::NoError ) {
            qWarning() << "File download failed with error" << m_reply->errorString();
            m_hasError = true;
        }

        finish();
    }

    void onOpenFileError()
    {
        m_hasError = true;
        finish();
    }

private:
    void finish()
    {
        m_file.close();
        m_finished = true;
        emit finished();
    }

    QFile m_file;
    QNetworkAccessManager m_manager;
    QNetworkReply *m_reply;

    bool m_finished;
    bool m_hasError;
};

#ifdef Q_OS_ANDROID

bool AssetDownloader::downloadAssets()
{
    const QString assetFilePath = ASSETS_PATH + m_fileName;
    const QString DOWNLOAD_PATH = QStringLiteral("http://www.kdab.com/~peppe/gl-training-assets/");
    const QString HASH_FILE_SUFFIX = QStringLiteral(".sha1sum");
    const bool assetFileExists = QFile::exists( assetFilePath );

    const QByteArray upstreamAssetFileHash = downloadAssetFileHash( QUrl( DOWNLOAD_PATH + m_fileName + HASH_FILE_SUFFIX ) );

    // some sort of download error. Ignore it if we have a local file
    if ( upstreamAssetFileHash.isNull() ) {
        if ( assetFileExists ) {
            qWarning() << "Could not check the validity of the local asset files, please check your network connection";
            return true;
        } else {
            return false;
        }
    }

    if ( assetFileExists ) {
        const QByteArray localAssetFileHash = calculateAssetFileHash( assetFilePath );
        if ( upstreamAssetFileHash == localAssetFileHash )
            return true;

        qWarning() << "Upstream hash is different from local file hash: downloading the assets file";
        qDebug() << "Upstream hash:" << upstreamAssetFileHash.toHex();
        qDebug() << "Local hash:" << localAssetFileHash.toHex();
    } else {
        qDebug() << "The local assets file doesn't exist, downloading it";
    }

    if ( !QDir().exists( ASSETS_PATH ) && !QDir().mkpath( ASSETS_PATH ) ) {
        qWarning() << "Could not create the assets path dir" << ASSETS_PATH;
        return false;
    }

    QEventLoop eventLoop;
    FileDownloader downloader( QUrl( DOWNLOAD_PATH + m_fileName ), ASSETS_PATH + m_fileName );

    connect( &downloader, &FileDownloader::finished, &eventLoop, &QEventLoop::quit );
    eventLoop.exec();

    if ( downloader.hasError() ) {
        qWarning() << "Error while downloading the asset files";
        return false;
    }

    const QByteArray localAssetFileHash = calculateAssetFileHash( assetFilePath );
    if ( localAssetFileHash != upstreamAssetFileHash ) {
        qWarning() << "The hash of the downloaded asset file does not match the hash of the upstream file";
        qDebug() << "Upstream hash:" << upstreamAssetFileHash.toHex();
        qDebug() << "Local hash:" << localAssetFileHash.toHex();
        return false;
    }

    return true;
}

#else

bool AssetDownloader::downloadAssets()
{
    return true;
}

#endif

#include "assetdownloader.moc"
