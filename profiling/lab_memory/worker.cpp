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
        if (!mail.isValid) {
            std::cout << "mail is not valid!" << std::endl;
            continue;
        }
        std::cout << mail.dateTime << ":" << mail.subject << "(" << mail.from << ")" << std::endl;
        for (const std::pair<std::string, int> &wc : mail.wordCount) {
            std::cout << "   " << wc.first << ": " << wc.second << std::endl;
        }
        std::cout << mail.words << " words counted" << std::endl;
    }
}
