/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "setup.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QCoreApplication>
#include <QVariant>

#include <initializer_list>

namespace Setup {
QSqlDatabase createDatabase()
{
    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    if (!db.isValid())
        return db;

    const QString databaseFile = qApp->applicationDirPath()
        + QLatin1String("/database.sqlite");
    QFile::remove(databaseFile);
    db.setDatabaseName(databaseFile);
    db.open();
    return db;
}

bool createTables(const QSqlDatabase &db)
{
    auto queries = {
        QStringLiteral("CREATE TABLE users (last_login DATETIME, num_logins INTEGER)"),
    };
    for (const auto& query : queries) {
        if (db.exec(query).lastError().isValid())
            return false;
    }
    return true;
}

bool fillTables(const QSqlDatabase &db, int maxUsers)
{
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral("INSERT INTO users (ROWID, last_login, num_logins) VALUES (:id, :last_login, :num_logins)"))) {
        return false;
    }
    for (int i = 0; i < maxUsers; ++i) {
        query.bindValue(QStringLiteral(":id"), i + 1);
        query.bindValue(QStringLiteral(":last_login"), 0);
        query.bindValue(QStringLiteral(":num_logins"), 0);
        if (!query.exec())
            return false;
    }
    return true;
}
}
