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
#include <unordered_map>

namespace {

bool startsWith(const std::string& haystack, const std::string& needle)
{
    return haystack.compare(0, needle.size(), needle) == 0;
}

void countWords(const std::string& line, Mail &mail)
{
    std::size_t start = 0;
    std::size_t end = 0;

    auto &wc = mail.wordCount;

    while (true) {
        bool last = false;

        // split line into words separated by ' '
        end = line.find(' ', start);
        if (end == std::string::npos) {
            end = line.size();
            last = true;
        }

        if (end != start) {
            // extract the word
            std::string word = line.substr(start, end - start);

            // check if there is an entry for this word in the word count list
            const auto it = std::lower_bound(wc.begin(), wc.end(), word,
                [](const std::pair<std::string,unsigned> &lhs, const std::string &rhs)
                {
                    return lhs.first < rhs;
                });

            if (it != wc.end() && (*it).first == word) { // word already in list -> increase counter
                (*it).second++;
            } else { // word not yet in list -> add it now
                wc.emplace(it, std::make_pair(std::move(word), 1));
            }
        }

        start = end + 1;

        if (last)
            break;
    }
}

class StringInterner
{
public:
    std::shared_ptr<std::string> intern(const std::string &text)
    {
        auto& interned = m_knownStrings[text];
        if (!interned)
            interned = std::make_shared<std::string>(text);
        return interned;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<std::string>> m_knownStrings;
};

}

MailParser::MailParser()
{
}

MailList MailParser::parse(const std::string& filePath)
{
    MailList mails;
    // We can expect around 500 emails in the mbox file, therefor we reserve
    // space here. Note that this approach will waste some memory if there are significant
    // less emails in the mbox file.
    // In real-world code, this should probably apply a heuristic
    // to find some number based on the file size or similar.
    mails.reserve(500);

    std::ifstream stream;
    stream.open(filePath);
    if (!stream.is_open()) {
        std::cerr << "Failed to parse mail file " << filePath << '\n';
        return mails;
    }

    // Create these strings only once and not on every loop iteration
    const std::string fromMarker("From: ");
    const std::string dateMarker("Date: ");
    const std::string subjectMarker("Subject: ");
    const std::string fromMilianMarker("From: milian.wolff at kdab.com (Milian Wolff)");

    StringInterner knownSenders;
    StringInterner knownSubjects;

    Mail currentMail;
    std::string line;
    unsigned mailsByMilian = 0;

    while (true) {
        if (!std::getline(stream, line)) {
            break;
        }

        countWords(line, currentMail);

        if (startsWith(line, fromMarker)) {
            currentMail.from = knownSenders.intern(line.substr(fromMarker.size()));
            if (startsWith(line, fromMilianMarker)) {
                ++mailsByMilian;
            }
        } else if (startsWith(line, dateMarker)) {
            currentMail.dateTime = line.substr(dateMarker.size());
        } else if (startsWith(line, subjectMarker)) {
            currentMail.subject = knownSubjects.intern(line.substr(subjectMarker.size()));

            mails.push_back(std::move(currentMail));
            currentMail = {};

            // Each email has around 200 distinct words, so reserve the necessary
            // space in advance to avoid repeating reallocations.
            // In real-world code, this should probably apply a heuristic
            // to find some number based on the file size or similar.
            currentMail.wordCount.reserve(200);
        }
    }

    std::cout << mailsByMilian << " written by Milian" << std::endl;

    return mails;
}
