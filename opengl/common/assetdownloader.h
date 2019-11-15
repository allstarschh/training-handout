#ifndef ASSETDOWNLOADER_H
#define ASSETDOWNLOADER_H

#include <QObject>
#include <QString>

#include "resources.h"

class AssetDownloader : public QObject
{
    Q_OBJECT
public:
    explicit AssetDownloader( const QStringList &fileName = ASSET_FILE_NAMES, QObject *parent = nullptr );

public slots:
    bool downloadAssets();

private:
    QStringList m_fileNames;
};

#endif // ASSETDOWNLOADER_H
