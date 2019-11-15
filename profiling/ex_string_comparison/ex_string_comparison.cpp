/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QString>
#include <QTextStream>
#include <iostream>

static QStringList getHaystack()
{
    QStringList haystack;
    for (int i = 0; i < 10000; ++i) {
        haystack << QString::number(i);
    }
    return haystack;
}


int main()
{
    const QStringList haystack = getHaystack();

    uint matches = 0;
    for (int i = 0; i < 1000; ++i) {
//--> slide
        for (const QString &str : haystack) {
            if (str == QString("42")) {
                ++matches;
            }
        }
//<-- slide
    }

    std::cout << "Matches: " << matches << std::endl;

    return 0;
}
