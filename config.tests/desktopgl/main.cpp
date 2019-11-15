/*************************************************************************
 *
 * Copyright (c) 2013-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QtOpenGL>

int main()
{
  // check if Qt was built with support for Desktop GL, by using one of the
  // constants defined in gl.h
  return GL_PROJECTION;
}
