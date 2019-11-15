/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QApplication>
#include <QCommandLineParser>

#include "mandelbrotwidget.h"
#include "mandelbrot.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption benchmarkOption(QStringList() << QStringLiteral("b") << QStringLiteral("benchmark"),
        QStringLiteral("Benchmark mode: Generate a certain number of mandelbrot sets repeatedly."),
        QStringLiteral("iterations"));
    parser.addOption(benchmarkOption);

    QCommandLineOption widthOption(QStringList() << QStringLiteral("w") << QStringLiteral("width"),
        QStringLiteral("Width of the image in benchmark mode."),
        QStringLiteral("width"));
    widthOption.setDefaultValue(QStringLiteral("640"));
    parser.addOption(widthOption);

    QCommandLineOption heightOption(QStringList() << QStringLiteral("h") << QStringLiteral("height"),
        QStringLiteral("Height of the image in benchmark mode."),
        QStringLiteral("height"));
    heightOption.setDefaultValue(QStringLiteral("480"));
    parser.addOption(heightOption);

    QCommandLineOption maxIterationsOption(QStringList() << QStringLiteral("i") << QStringLiteral("max-iterations"),
        QStringLiteral("Maximum number of iterations in the Mandelbroth algorithm in benchmark mode."),
        QStringLiteral("iterations"));
    maxIterationsOption.setDefaultValue(QStringLiteral("200"));
    parser.addOption(maxIterationsOption);

    parser.process(app.arguments());

    if (parser.isSet(benchmarkOption)) {
        const int width = parser.value(widthOption).toUInt();
        const int height = parser.value(widthOption).toUInt();
        const int runs = parser.value(benchmarkOption).toUInt();
        const int maxIterations = parser.value(maxIterationsOption).toUInt();
        QImage image(width, height, QImage::Format_RGB32);
        for (int i = 0; i < runs; ++i) {
            drawMandelbrot(&image, maxIterations);
        }
        return 0;
    }

    MandelBrotWidget widget;
    widget.show();
    return app.exec();
}
