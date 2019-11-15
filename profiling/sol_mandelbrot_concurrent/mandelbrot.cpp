/*************************************************************************
 *
 * Copyright (c) 2016-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "mandelbrot.h"

#include <QImage>
#include <QColor>
#include <QtConcurrentRun>
#include <QThreadPool>
#include <QAtomicInt>
#include <QGradientStops>
#include <QDebug>

#include <atomic>
#include <vector>
#include <complex>

namespace {
inline double fastNorm(std::complex<double> c)
{
    return c.real() * c.real() + c.imag() * c.imag();
}

inline std::complex<double> square(std::complex<double> c)
{
    return std::complex<double>(c.real() * c.real() - c.imag() * c.imag(), 2. * c.real() * c.imag());
}

QGradientStops generateGradient()
{
    QGradientStops ret;
    ret.resize(6);
    // taken from http://stackoverflow.com/a/25816111/35250
    ret[0] = qMakePair(0.0, QColor(0, 7, 100));
    ret[1] = qMakePair(0.16, QColor(32, 107, 203));
    ret[2] = qMakePair(0.42, QColor(237, 255, 255));
    ret[3] = qMakePair(0.6425, QColor(255, 170, 0));
    ret[4] = qMakePair(0.8575, QColor(0, 2, 0));
    ret[5] = qMakePair(1.0, QColor(0, 0, 0));
    return ret;
}

/**
 * Interpolated color profile which works much better for zoomed views
 */
inline QRgb generateColor(uint iterations, uint max_iterations)
{
    double hue = double(iterations) / max_iterations;

    static const QGradientStops stops = generateGradient();
    for (int i = 1; i < stops.size() - 1; ++i) {
        const QGradientStop stop = stops[i];
        if (stop.first >= hue) {
            const QGradientStop previousStop = stops[i - 1];
            const double distance = stop.first - previousStop.first;
            const double ratio = (stop.first - hue) / distance;
            return qRgb(stop.second.red() * ratio + previousStop.second.red() * (1. - ratio),
                        stop.second.green() * ratio + previousStop.second.green() * (1. - ratio),
                        stop.second.blue() * ratio + previousStop.second.blue() * (1. - ratio));
        }
    }

    return qRgb(0, 0, 0);
}

// static data that can be concurrently accessed from multiple threads
struct MandelBrotData
{
    MandelBrotData(QImage *image, uint max_iterations, const QRectF &rect)
        : max_iterations(max_iterations)
        , width(image->width())
        , height(image->height())
    {
        cxmin = rect.left();
        const double cxmax = rect.right();
        cymin = rect.top();
        const double cymax = rect.bottom();

        x_scale = (cxmax - cxmin) / (width - 1);
        y_scale = (cymax - cymin) / (height - 1);
    }

    uint max_iterations;

    int width;
    int height;
    double cxmin;
    double cymin;
    double x_scale;
    double y_scale;
};

// mutable data that needs to be synchronized when accessed from multiple threads
struct SharedMandelBrotData
{
    SharedMandelBrotData(const MandelBrotData& data)
        : histogram(data.max_iterations + 1)
        , total(0)
    {
        std::fill(histogram.begin(), histogram.end(), 0);
    }

    std::vector<std::atomic<uint>> histogram;
    std::atomic<uint> total;
};

/**
 * First pass of the histogram coloring algorithm
 * cf.: https://en.wikipedia.org/wiki/Mandelbrot_set#Histogram_coloring
 *
 * - compute the iteration histogram
 * - count total number of iterations
 */
void firstPassMandelbrotRow(const MandelBrotData& data, SharedMandelBrotData* shared,
                            int row, uint* scanline)
{
    uint* iterationsIt = scanline;

    const double cy = data.cymin + row * data.y_scale;

    for (int x = 0; x < data.width; ++x, ++iterationsIt) {
        const double cx = data.cxmin + x * data.x_scale;
        const std::complex<double> c(cx, cy);

        std::complex<double> z(0);
        uint iterations = 0;
        for (; iterations < data.max_iterations && fastNorm(z) < 4.0; ++iterations) {
            z = square(z) + c;
        }

        ++shared->histogram[iterations];
        ++shared->total;
        *iterationsIt = iterations;
    }
}

/**
 * Second pass of the histogram coloring algorithm
 * cf.: https://en.wikipedia.org/wiki/Mandelbrot_set#Histogram_coloring
 *
 * - colorize the image based on the first-pass iteration histogram
 */
void secondPassMandelbrotRow(QImage *image, const SharedMandelBrotData* shared,
                             int width, int row, const uint* scanline)
{
    const uint* iterationsIt = scanline;
    QRgb* pixel = reinterpret_cast<QRgb*>(image->scanLine(row));
    const QRgb* const pixelEnd = pixel + width;

    for (; pixel != pixelEnd; ++pixel, ++iterationsIt) {
        const uint iterations = *iterationsIt;
        uint hue = 0;
        for (uint i = 0; i < iterations; ++i) {
            hue += shared->histogram[i].load();
        }
        *pixel = generateColor(hue, shared->total.load());
    }
}
}

// based on http://rosettacode.org/wiki/Mandelbrot_set#C.2B.2B
void drawMandelbrot(QImage *image, uint max_iterations)
{
#ifndef Q_COMPILER_THREADSAFE_STATICS
    // Initialize static data before accessing it from multiple threads.
    // This is only required for compilers that do not conform to the C++11
    // standard, such as Visual Studio 2013
    generateColor(0, 1);
#endif

    QRectF rect(0, 0, 0.05, 0.01);
    rect.moveCenter(QPointF(-0.088, 0.654));

    MandelBrotData data(image, max_iterations, rect);
    SharedMandelBrotData shared(data);
    std::vector<uint> iterations(data.width * data.height, 0);
    // every thread will work on its own scanline
    // that allows us to access and mutate that data without any synchronization
    auto scanline = [&] (int row) {
        return iterations.data() + (data.width * row);
    };

    // we use the global thread pool, instead of recreating the pools every time
    // otherwise, at least do not hardcode the max thread count, but rather use
    // QThread::idealThreadCount() to obtain a fitting number of threads

    // step 1: run the first pass of our algorithm
    std::vector<QFuture<void> > futures(data.height);
    for (int y = 0; y < data.height; ++y) {
        futures[y] = QtConcurrent::run(firstPassMandelbrotRow,
                                       data, &shared, y, scanline(y));
    }

    // step 2: now we need to wait for the first pass to finish
    //         before we can continue with the second pass
    for (int y = 0; y < data.height; ++y) {
        futures[y].waitForFinished();
    }

    // step 3: run the second pass of our algorithm
    for (int y = 0; y < data.height; ++y) {
        futures[y] = QtConcurrent::run(secondPassMandelbrotRow,
                                       image, &shared, data.width, y, scanline(y));
    }

    // step 4: now we need to wait again
    for (int y = 0; y < data.height; ++y) {
        futures[y].waitForFinished();
    }

    // now our image is finished
}
