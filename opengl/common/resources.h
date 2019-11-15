#ifndef RESOURCES_H
#define RESOURCES_H

#include <QDebug>
#include <QResource>

#define OPENGL_XSTRINGIFY(s) OPENGL_STRINGIFY(s)
#define OPENGL_STRINGIFY(s) #s

#ifdef Q_OS_ANDROID
static const QString ASSETS_PATH = QStringLiteral( "/sdcard/kdab_examples/" );
#else
static const QString ASSETS_PATH = QStringLiteral( OPENGL_XSTRINGIFY( ASSETS ) );
#endif

static const QStringList ASSET_FILE_NAMES =
    (QStringList() << QStringLiteral( "opengl-assets.qrb" )
                   << QStringLiteral( "extra.qrb" ) );

inline bool initializeAssetResources( const QStringList &fileNames = ASSET_FILE_NAMES )
{
    bool b = true;
    foreach (const QString &fileName, fileNames) {
        const QString assetPath = ASSETS_PATH + fileName;
        qDebug() << "assetPath =" << assetPath;
        b &= QResource::registerResource( assetPath );
        if ( !b )
            qDebug() << "Failed to load assets from" << fileName;
    }
    return b;
}

#endif // RESOURCES_H
