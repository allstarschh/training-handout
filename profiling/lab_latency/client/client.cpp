/*************************************************************************
 *
 * Copyright (c) 2017-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "client.h"

#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>

#ifdef HAVE_LTTNG
#include <lttng/tracef.h>
#else
#define tracef(...)
#endif

namespace {
QSqlRecord findUser(const QSqlDatabase &db, int userId)
{
    tracef("begin find user %d", userId);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral("SELECT * FROM users WHERE ROWID = :id")))
        return {};

    query.bindValue(QStringLiteral(":id"), userId);
    if (!query.exec() || !query.next())
        return {};

    tracef("end find user %d", userId);

    return query.record();
}

void login(const QSqlDatabase &db, int userId, int numLogins, const QString &lastLogin)
{
    tracef("begin login %d", userId);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral("UPDATE users SET num_logins = :num_logins, last_login = :last_login WHERE ROWID = :id")))
        return;
    query.bindValue(QStringLiteral(":id"), userId);
    query.bindValue(QStringLiteral(":num_logins"), numLogins);
    query.bindValue(QStringLiteral(":last_login"), lastLogin);
    query.exec();
    tracef("end login %d", userId);
}
}

Client::Client(const QSqlDatabase &db, int userId, QObject *parent)
    : QObject(parent)
    , m_db(db)
    , m_userId(userId)
{
}

Client::~Client() = default;

void Client::run()
{
    // use write-ahead locking for improved performance
//     m_db.exec(QStringLiteral("PRAGMA journal_mode=WAL"));

    auto user = findUser(m_db, m_userId);
    QTextStream out(stdout);
    out << "Client request for user id:" << m_userId
        << "\n\tLogins:" << user.value(QStringLiteral("num_logins")).toInt()
        << "\n\tLast login:" << user.value(QStringLiteral("last_login")).toString()
        << endl;

    // TODO: slower queries without indices?
    login(m_db, m_userId,
          user.value(QStringLiteral("num_logins")).toInt() + 1,
          QDateTime::currentDateTime().toString(Qt::RFC2822Date));

    emit finished();
}
