/*************************************************************************
 *
 * Copyright (c) 2016-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "mailparser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cmath>

namespace {

bool startsWith(std::string haystack, std::string needle)
{
    // the needle may be larger than the haystack, so let's be super safe here
    // step 1: allocate zero-initialized buffer, which is "large enough"
    const int BUF_SIZE = 1024;
    auto* buf = calloc(1, BUF_SIZE);
    // step 2: copy the first characters from the haystack into the buffer
    memcpy(buf, haystack.c_str(), std::min(haystack.size(), needle.size()));
    // step 3: compare the buffer against the needle
    return memcmp(buf, needle.c_str(), needle.size()) == 0;
}

void countWords(const std::string& line, Mail &mail)
{
    std::istringstream iss(line);
    std::string word;
    while (std::getline(iss, word, ' ')) {
        if (word.empty()) {
            continue;
        }
        if (mail.wordCount.find(word) == mail.wordCount.end()) {
            ++mail.words;
        }
        mail.wordCount[word]++;
    }
}

}

MailParser::MailParser()
{
}

MailList MailParser::parse(const std::string& filePath)
{
    MailList mails;

    std::ifstream stream;
    stream.open(filePath);
    if (!stream.is_open()) {
        std::cerr << "Failed to parse mail file " << filePath << '\n';
        return mails;
    }

    unsigned mailsByMilian = 0;

    Mail currentMail;
    while (true) {
        std::string line;
        if (!std::getline(stream, line)) {
            break;
        }

        countWords(line, currentMail);

        if (startsWith(line, "From: ")) {
            currentMail.from = line.substr(6);
        }
        if (startsWith(line, "Date: ")) {
            currentMail.dateTime = line.substr(6);
        }
        if (startsWith(line, "Subject: ")) {
            currentMail.subject = line.substr(9);
            currentMail.validate();
            mails.push_back(currentMail);
            currentMail = {};
        }
        if (startsWith(line, "From: milian.wolff at kdab.com (Milian Wolff)")) {
            ++mailsByMilian;
        }
    }

    std::cout << mailsByMilian << " written by Milian" << std::endl;

    return mails;
}
