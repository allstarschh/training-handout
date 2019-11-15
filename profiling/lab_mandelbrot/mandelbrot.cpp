/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "mandelbrot.h"

#include <QImage>
#include <QColor>

#include <complex>

namespace {
inline QRgb generateColor(uint iterations, uint max_iterations)
{
    if (iterations == max_iterations) {
        return qRgb(0, 0, 0);
    }
    QColor color = QColor::fromHsvF(double(iterations) / max_iterations, 1.0, 1.0);
    return color.rgb();
}
}

// based on http://rosettacode.org/wiki/Mandelbrot_set#C.2B.2B
void drawMandelbrot(QImage *image, uint max_iterations)
{
    const double cxmin = -2.5;
    const double cxmax = 1.0;
    const double cymin = -1.0;
    const double cymax = +1.0;

    for (int x = 0; x < image->width(); ++x) {
        for (int y = 0; y < image->height(); ++y) {
            std::complex<double> z(0);
            unsigned int iterations = 0;
            for (; iterations < max_iterations && std::abs(z) < 2.0; ++iterations) {
                const double cx = cxmin + x * (cxmax - cxmin) / (image->width() - 1);
                const double cy = cymin + y * (cymax - cymin) / (image->height() - 1);
                const std::complex<double> c(cx, cy);

                z = z*z + c;
            }

            image->setPixel(x, y, generateColor(iterations, max_iterations));
        }
    }
}
