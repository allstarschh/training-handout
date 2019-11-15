/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#ifdef HAVE_LTTNG
#include <lttng/tracef.h>
#define LTTNG_TRACEF(...) tracef(__VA_ARGS__)
#else
#define LTTNG_TRACEF(...)
#endif

void list(const QFileInfo& entry)
{
    if (entry.isSymLink()) {
        // don't follow symlinks, otherwise we might end up recursing infinitely
        qDebug() << entry.absoluteFilePath() << "->" << entry.symLinkTarget();
        return;
    }

    if (entry.isDir()) {
        (qDebug() << entry.absoluteFilePath()).nospace() << "/";
        auto dir = entry.dir();
        dir.cd(entry.fileName());
        auto filter = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden;
        const QFileInfoList entryList = dir.entryInfoList(filter);
        for (const auto& childEntry : entryList) {
            list(childEntry);
        }
    } else {
        qDebug() << entry.absoluteFilePath();
    }

}

QtMessageHandler originalHandler = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext& context,
                    const QString& message)
{
    LTTNG_TRACEF("log(%d) %s:%d: %s", type, context.file, context.line,
                 qPrintable(message));
    originalHandler(type, context, message);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    originalHandler = qInstallMessageHandler(messageHandler);

    auto paths = app.arguments();
    if (paths.size() == 1) {
        paths.push_back(app.applicationDirPath() + QLatin1String("/../"));
    }

    bool first = true;
    for (const auto& path : paths) {
        if (first) {
            first = false;
            continue;
        }
        list(QFileInfo(path));
    }
    return 0;
}
