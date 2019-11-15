/*************************************************************************
 *
 * Copyright (c) 2016-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "mandelbrotwidget.h"

#include "mandelbrot.h"

#include <QPaintEvent>
#include <QPainter>

MandelBrotWidget::MandelBrotWidget(QWidget* parent)
    : QWidget(parent)
{
}

void MandelBrotWidget::paintEvent(QPaintEvent* event)
{
    const QRect rect = event->rect();

    QImage image(rect.width(), rect.height(), QImage::Format_RGB32);
    drawMandelbrot(&image);

    QPainter painter(this);
    painter.drawImage(rect, image);
}
