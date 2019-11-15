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

    // advance plotter time in ~60Hz intervals
    QElapsedTimer timer;
    QTimer animationDriver;
    animationDriver.setInterval(16);
    QObject::connect(&animationDriver, &QTimer::timeout, plotter, [&]() {
        plotter->setTime(timer.elapsed());
    });

    // start animation when button is clicked
    QObject::connect(button, &QPushButton::clicked, &app, [&](){
        const auto wasActive = animationDriver.isActive();
        if (!wasActive) {
            // start time measurement and animation timer
            timer.start();
            animationDriver.start();
        } else {
            // stop animation timer
            animationDriver.stop();
        }
        Q_ASSERT(wasActive != animationDriver.isActive());
    });

    // center on screen
    window.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                           QSize(400, 400),
                                           app.desktop()->availableGeometry()));
    window.show();

    return app.exec();
}
