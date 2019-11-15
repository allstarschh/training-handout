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

#include <vector>
#include <complex>

namespace {
enum {
    COLOR_MAP_SIZE = 1000
};

inline std::vector<QRgb> generateColorMap()
{
    std::vector<QRgb> ret(COLOR_MAP_SIZE + 1);
    for (int i = 0; i < COLOR_MAP_SIZE; ++i) {
        QColor color = QColor::fromHsvF(double(i) / COLOR_MAP_SIZE, 1.0, 1.0);
        ret[i] = color.rgb();
    }
    ret[COLOR_MAP_SIZE] = qRgb(0, 0, 0);
    return ret;
}

inline QRgb generateColor(uint iterations, uint max_iterations)
{
    static const std::vector<QRgb> palette = generateColorMap();
    return palette[(iterations * COLOR_MAP_SIZE) / max_iterations];
}

inline double fastNorm(std::complex<double> c)
{
    return c.real() * c.real() + c.imag() * c.imag();
}

inline std::complex<double> square(std::complex<double> c)
{
    return std::complex<double>(c.real() * c.real() - c.imag() * c.imag(), 2. * c.real() * c.imag());
}
}

// based on http://rosettacode.org/wiki/Mandelbrot_set#C.2B.2B
void drawMandelbrot(QImage *image, uint max_iterations)
{
    const double cxmin = -2.5;
    const double cxmax = 1.0;
    const double cymin = -1.0;
    const double cymax = +1.0;

    const int width = image->width();
    const int height = image->height();

    const double x_scale = (cxmax - cxmin) / (width - 1);
    const double y_scale = (cymax - cymin) / (height - 1);

    for (int y = 0; y < height; ++y) {
        const double cy = cymin + y * y_scale;
        QRgb* scanLine = reinterpret_cast<QRgb*>(image->scanLine(y));
        for (int x = 0; x < width; ++x) {
            const double cx = cxmin + x * x_scale;
            const std::complex<double> c(cx, cy);

            std::complex<double> z(0);
            unsigned int iterations = 0;
            for (; iterations < max_iterations && fastNorm(z) < 4.0; ++iterations) {
                z = square(z) + c;
            }

            scanLine[x] = generateColor(iterations, max_iterations);
        }
    }
}
