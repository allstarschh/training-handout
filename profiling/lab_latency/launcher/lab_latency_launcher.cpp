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
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

#include "setup.h"
#include "launcher.h"

namespace {
namespace Defaults {
const int numClients = 100;
const int numParallelClients = 10;
const int numMaxUsers = 10;
}
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;

    QCommandLineOption clientsOption({QStringLiteral("c"), QStringLiteral("clients")},
        QStringLiteral("Number of client requests to launch in total, defaults to %1.")
            .arg(Defaults::numClients),
        QStringLiteral("N"), QString::number(Defaults::numClients));
    parser.addOption(clientsOption);

    QCommandLineOption parallelClientsOption({QStringLiteral("p"), QStringLiteral("parallel")},
        QStringLiteral("Number of client requests to launch in parallel, defaults to %1.")
            .arg(Defaults::numParallelClients),
        QStringLiteral("N"), QString::number(Defaults::numParallelClients));
    parser.addOption(parallelClientsOption);

    QCommandLineOption maxUsersOptions({QStringLiteral("u"), QStringLiteral("users")},
        QStringLiteral("Number of distinct users to use by client requests, defaults to %1.")
            .arg(Defaults::numMaxUsers),
        QStringLiteral("N"), QString::number(Defaults::numMaxUsers));
    parser.addOption(maxUsersOptions);

    parser.addHelpOption();

    parser.process(app.arguments());

    auto db = Setup::createDatabase();
    if (!db.isOpen()) {
        qWarning() << "Failed to create database" << db.lastError();
        return 1;
    }

    if (!Setup::createTables(db)) {
        qWarning() << "Failed to create tables" << db.lastError();
        return 1;
    }

    const int numMaxUsers = parser.value(maxUsersOptions).toInt();
    if (!Setup::fillTables(db, numMaxUsers)) {
        qWarning() << "Failed to fill tables" << db.lastError();
        return 1;
    }

    const int numClients = parser.value(clientsOption).toInt();
    const int numParallelClients = parser.value(parallelClientsOption).toInt();

    Launcher launcher(numClients, numParallelClients, numMaxUsers,
                      db.databaseName());
    QObject::connect(&launcher, &Launcher::finished,
                     &app, &QCoreApplication::quit);
    launcher.run();
    return app.exec();
}
