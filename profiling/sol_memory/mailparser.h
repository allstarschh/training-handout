/*************************************************************************
 *
 * Copyright (c) 2016-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef MAILPARSER_H
#define MAILPARSER_H

#include <memory>
#include <string>
#include <vector>

struct Mail
{
    std::shared_ptr<std::string> from;
    std::string dateTime;
    std::shared_ptr<std::string> subject;
    std::vector<std::pair<std::string, unsigned>> wordCount;

    bool isValid() const
    {
        return from && subject && !dateTime.empty();
    }
};
typedef std::vector<Mail> MailList;

class MailParser
{
public:
    MailParser();

    MailList parse(const std::string &filePath);
};

#endif // MAILPARSER_H
