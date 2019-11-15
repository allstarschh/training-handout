/*************************************************************************
 *
 * Copyright (c) 2017-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "launcher.h"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

#include <cstdlib>

namespace {
QString clientExecutable()
{
    return qApp->applicationDirPath() + QLatin1String("/../client/lab_latency_client");
}
}

Launcher::Launcher(int numClients, int numParallelClients, int maxUsers,
                   const QString &databasePath, QObject *parent)
    : QObject(parent)
    , m_numClients(numClients)
    , m_numParallelClients(numParallelClients)
    , m_launchedClients(0)
    , m_maxUsers(maxUsers)
    , m_databasePath(databasePath)
{
}

Launcher::~Launcher()
{
}

void Launcher::run()
{
    // request launching the first batch of parallel clients
    // this goes through Qt's event loop to ensure we don't
    // call this recursively
    QMetaObject::invokeMethod(this, "launchParallelClients", Qt::QueuedConnection);
}

void Launcher::launchParallelClients()
{
    while (m_runningClients.size() != m_numParallelClients
            && m_launchedClients != m_numClients)
    {
        auto client = new QProcess(this);
        client->setProcessChannelMode(QProcess::ForwardedChannels);
        m_runningClients.append(client);
        ++m_launchedClients;

        auto handler = [client, this](){
            if (client->error() != QProcess::UnknownError) {
                qWarning() << "Client failed:" << client->error()
                           << client->errorString();
            } else if (client->exitStatus() != QProcess::NormalExit
                || client->exitCode() != EXIT_SUCCESS)
            {
                qWarning() << "Client failed:" << client->exitStatus()
                           << client->exitCode();
            }

            m_runningClients.removeOne(client);
            client->deleteLater();
            run();
        };
        connect(client, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
                this, handler);
        connect(client, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                this, handler);

        client->start(clientExecutable(), {
            QStringLiteral("--database"), m_databasePath,
            QStringLiteral("--user"), QString::number(m_launchedClients % m_maxUsers + 1)
        });
    }

    if (m_launchedClients == m_numClients && m_runningClients.isEmpty()) {
        emit finished();
    }
}
