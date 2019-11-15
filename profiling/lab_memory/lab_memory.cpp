/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "worker.h"

#include <QCommandLineParser>
#include <QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption benchmarkOption(QStringList() << QStringLiteral("b") << QStringLiteral("benchmark"),
        QStringLiteral("Benchmark mode: Load the input file a certain number of times and then quit."),
        QStringLiteral("iterations"));
    parser.addOption(benchmarkOption);
    parser.addHelpOption();

    parser.process(app.arguments());

    Worker worker;

    if (parser.isSet(benchmarkOption)) {
        for (int i = 0, c = parser.value(benchmarkOption).toInt(); i < c; ++i) {
            worker.loadInput();
        }
        return 0;
    }

    worker.loadInput();

    return 0;
}
