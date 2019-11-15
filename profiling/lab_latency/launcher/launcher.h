/*************************************************************************
 *
 * Copyright (c) 2017-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QVector>

class QProcess;

class Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(int numClients, int numParallelClients, int maxUsers,
                      const QString &databasePath,
                      QObject *parent = nullptr);
    ~Launcher();

    void run();

private slots:
    void launchParallelClients();

signals:
    void finished();

private:
    int m_numClients;
    int m_numParallelClients;
    int m_launchedClients;
    int m_maxUsers;
    QString m_databasePath;
    QVector<QProcess*> m_runningClients;
};

#endif // LAUNCHER_H
