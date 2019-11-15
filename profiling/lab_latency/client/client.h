/*************************************************************************
 *
 * Copyright (c) 2017-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QSqlDatabase>

class QProcess;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QSqlDatabase &db, int userId,
                    QObject *parent = nullptr);
    ~Client();

public slots:
    void run();

signals:
    void finished();

private:
    QSqlDatabase m_db;
    int m_userId;
};

#endif // CLIENT_H
