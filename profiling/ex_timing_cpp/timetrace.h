/*************************************************************************
 *
 * Copyright (c) 2017-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef TIMETRACE_H
#define TIMETRACE_H

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <map>

/**
 * A simple class to use for manual time tracing/logging.
 *
 * It aggregates all values for the key and reports the total numbers
 * on destruction. The values are sorted in ascending order, such that
 * the most important values are shown at the end of the app's output.
 *
 * Example usage:
 * \code
 * void match(const QRegExp& regex) {
 *     static TimeTrace<QString> trace("QRegExp Matching");
 *     auto start = trace.start();
 *     regex.indexIn(...);
 *     trace.end(start, regex.pattern());
 * }
 * \endcode
 *
 * Example output:
 * \verbatim
    ###### Time Trace for File Parsing ######
    readLine: 0.0057468s over 28130 calls, i.e. 2.04294e-07s per call
    QRegularExpression: 0.0128938s over 28130 calls, i.e. 4.58366e-07s per call
    QRegExp: 0.0186988s over 28130 calls, i.e. 6.64729e-07s per call
    ------ 0.0373394s elapsed in total over 84390 calls, i.e. 4.42463e-07s per call ######
 * \endverbatim
 */
template <typename Key>
struct TimeTrace
{
    using duration_t = std::chrono::nanoseconds;

    TimeTrace(const char* label)
        : m_label(label)
    {
        m_cost.reserve(100);
    }

    ~TimeTrace()
    {
        // sort by elapsed time before output
        std::multimap<duration_t, Key> sorted;
        // also aggregate total numbers
        Cost total;
        for (auto it = m_cost.begin(), end = m_cost.end(); it != end; ++it)
        {
            sorted.insert(std::make_pair(it->second.elapsed, it->first));
            total.elapsed += it->second.elapsed;
            total.calls += it->second.calls;
        }

        std::cout << "###### Time Trace for " << m_label << " ######\n";
        for (auto it = sorted.begin(), end = sorted.end(); it != end; ++it)
        {
            const auto& label = it->second;
            const float elapsed = it->first.count() * 1E-9;
            const auto calls = m_cost[label].calls;
            std::cout << label << ": " << elapsed << "s over " << calls << " calls, i.e. " << (elapsed / calls) << "s per call\n";
        }
        std::cout << "------ " << (total.elapsed.count() * 1E-9) << "s elapsed in total over " << total.calls << " calls"
                  << ", i.e. " << (total.elapsed.count() * 1E-9 / total.calls) << "s per call ######\n";
    }

    std::chrono::steady_clock::time_point start() const
    {
        return std::chrono::steady_clock::now();
    }

    void end(const std::chrono::steady_clock::time_point& t, const Key& key)
    {
        const auto end = start();
        const auto elapsed = std::chrono::duration_cast<duration_t>(end - t);
        Cost& cost = m_cost[key];
        cost.elapsed += elapsed;
        ++cost.calls;
    }

private:
    const char* m_label;
    struct Cost
    {
        Cost()
            : elapsed(0)
            , calls(0)
        {}
        duration_t elapsed;
        uint64_t calls;
    };
    std::unordered_map<Key, Cost> m_cost;
};

#endif
