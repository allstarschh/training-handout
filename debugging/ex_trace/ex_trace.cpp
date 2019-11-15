#include <QCoreApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (!QFile::exists(app.applicationDirPath() + QStringLiteral("/license"))) {
        qWarning() << "License file not found :(";
        return 1;
    }
    qDebug() << "Yay, found the license :)";
    return 0;
}
