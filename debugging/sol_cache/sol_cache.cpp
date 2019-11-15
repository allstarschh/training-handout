#include "plotwidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QElapsedTimer>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);


    QWidget window;
    window.setLayout(new QVBoxLayout);
    auto button = new QPushButton(QStringLiteral("Animate"), &window);
    window.layout()->addWidget(button);
    auto plotter = new PlotWidget(&window);
    window.layout()->addWidget(plotter);

    QElapsedTimer timer;
    QTimer animationDriver;
    animationDriver.setInterval(16);
    QObject::connect(&animationDriver, &QTimer::timeout, plotter, [&]() {
        plotter->advance(timer.elapsed());
        timer.start();
    });
    QObject::connect(button, &QPushButton::clicked, &app, [&](){
        if (!animationDriver.isActive()) {
            // start time measurement and animation timer
            timer.start();
            animationDriver.start();
        } else {
            // stop animation timer
            animationDriver.stop();
        }
    });

    // center on screen
    window.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                           QSize(400, 400),
                                           app.desktop()->availableGeometry()));
    window.show();

    return app.exec();
}
