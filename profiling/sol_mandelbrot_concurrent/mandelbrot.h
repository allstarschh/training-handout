/*************************************************************************
 *
 * Copyright (c) 2016-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef MANDELBROT_H
#define MANDELBROT_H

class QImage;

void drawMandelbrot(QImage *image, unsigned int max_iterations = 1000);

#endif // MANDELBROT_H
