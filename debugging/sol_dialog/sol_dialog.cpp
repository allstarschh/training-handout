/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "window.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Window window;
    // center on screen
    window.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                           window.size(),
                                           app.desktop()->availableGeometry()));
    window.show();

    return app.exec();
}
