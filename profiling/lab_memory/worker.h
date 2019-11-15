/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef WORKER_H
#define WORKER_H

#include <string>

class Worker
{
public:
    void loadInput();
    void parseFile(const std::string &filePath);
};

#endif // WORKER_H
