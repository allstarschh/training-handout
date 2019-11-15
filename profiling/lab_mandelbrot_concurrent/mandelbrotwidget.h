/*************************************************************************
 *
 * Copyright (c) 2016-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef MANDELBROTWIDGET_H
#define MANDELBROTWIDGET_H

#include <QWidget>

class MandelBrotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MandelBrotWidget(QWidget *parent = nullptr);

protected:
    /// Paint a slightly more elaborate mandelbrot image.
    void paintEvent(QPaintEvent *event) override;
};

#endif // MANDELBROTWIDGET_H
