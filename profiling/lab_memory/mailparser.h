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

#include <list>
#include <map>
#include <string>

struct Mail
{
    bool isValid = false;
    std::string from;
    std::string dateTime;
    std::string subject;
    std::map<std::string, unsigned> wordCount;
    unsigned words = 0;

    void validate()
    {
        isValid = !from.empty() && !dateTime.empty() && !subject.empty();
    }
};
typedef std::list<Mail> MailList;

class MailParser
{
public:
    MailParser();

    MailList parse(const std::string &filePath);
};

#endif // MAILPARSER_H
