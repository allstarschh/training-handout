/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <regex>
#include <fstream>
#include <iostream>
#include <string>

#include "timetrace.h"

void parse_file(std::ifstream& stream)
{
    const auto pattern = "\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,}\\b.";
    const auto regex_ecma = std::regex(pattern, std::regex::ECMAScript);
    const auto regex_ecma_optimized = std::regex(pattern, std::regex::ECMAScript | std::regex::optimize);

    static TimeTrace<const char*> trace("File Parsing");

    std::string line;
    while (stream.good()) {
        auto start = trace.start();
        std::getline(stream, line);
        trace.end(start, "readLine");

        start = trace.start();
        std::regex_match(line, regex_ecma);
        trace.end(start, "regex_match(ECMA)");

        start = trace.start();
        std::regex_match(line, regex_ecma_optimized);
        trace.end(start, "regex_match(ECMA|optimized)");
    }
}

int main()
{
    std::ifstream stream;
    stream.open(INPUT_FILE);
    if (!stream.is_open()) {
        std::cerr << "Failed to open input resource file: " << INPUT_FILE << "\n";
        return 1;
    }

    parse_file(stream);

    return 0;
}
