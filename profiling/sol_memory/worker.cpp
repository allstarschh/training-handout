/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "worker.h"

#include "mailparser.h"

#include <iostream>

void Worker::loadInput()
{
    parseFile(INPUT_FILE);
}

void Worker::parseFile(const std::string& filePath)
{
    MailParser parser;
    for (const auto &mail : parser.parse(filePath)) {
        if (!mail.isValid()) {
            std::cout << "mail is not valid!" << std::endl;
            continue;
        }
        std::cout << mail.dateTime << ":" << *mail.subject << "(" << *mail.from << ")\n";

        for (const auto &wc : mail.wordCount) {
            std::cout << "   " << wc.first << ": " << wc.second << "\n";
        }
        std::cout << mail.wordCount.size() << " words counted" << std::endl;
    }

    std::cout << std::flush;
}
