/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>

#include "client.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;

    QCommandLineOption databaseOption({QStringLiteral("d"), QStringLiteral("database")},
        QStringLiteral("Path to the SQLite database."),
        QStringLiteral("path"));
    parser.addOption(databaseOption);

    QCommandLineOption userOption({QStringLiteral("u"), QStringLiteral("user")},
        QStringLiteral("User ID"),
        QStringLiteral("ID"));
    parser.addOption(userOption);

    parser.addHelpOption();

    parser.process(app.arguments());

    if (!parser.isSet(databaseOption) || !parser.isSet(userOption)) {
        qWarning() << "missing CLI argument\n" << parser.helpText();
        return 1;
    }

    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    if (!db.isValid()) {
        qWarning() << "failed to add database:" << db.lastError();
        return 1;
    }

    db.setDatabaseName(parser.value(databaseOption));
    if (!db.open()) {
        qWarning() << "failed to open database:" << db.lastError();
        return 1;
    }

    Client client(db, parser.value(userOption).toInt());
    QObject::connect(&client, &Client::finished,
                     &app, &QCoreApplication::quit);
    QMetaObject::invokeMethod(&client, "run", Qt::QueuedConnection);
    return app.exec();
}
