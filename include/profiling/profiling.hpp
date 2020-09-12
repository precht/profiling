/**
 *  MIT License
 *
 *  Copyright (c) 2020 Jakub Precht <github.com/precht>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <sstream>
#include <cmath>
#include <type_traits>
#include <execution>
#include <utility>

#include "jpds/RedBlackTree.hpp"

namespace profiling {

/////////////////////////////////////////////////////////////////////////////// DECLARATION ////////////////////////////

class Stopwatch
{
public:
    void start();

    void lap();

    template<typename Duration>
    int64_t last() const;

    template<typename Duration>
    int64_t avg() const;

    template<typename Duration>
    int64_t median() const;

    template<typename Duration>
    int64_t stdev();

    template<typename Duration>
    std::string str();

    size_t size() const;

    void clear();

private:
    using Clock = std::chrono::steady_clock;
    using Pair = std::pair<Clock::duration, double>;

    Clock::time_point m_timepoint{};
    Clock::duration m_sum{};
    std::vector<Pair> m_array{};
    jpds::RedBlackTree<Clock::duration> m_tree{};
};

/////////////////////////////////////////////////////////////////////////////// DEFINITION /////////////////////////////

template<typename Duration> inline
std::string Stopwatch::str()
{
    const auto unit = [&]() {
        if constexpr (std::is_same<Duration, std::chrono::hours>::value)
            return "h";
        else if constexpr (std::is_same<Duration, std::chrono::minutes>::value)
            return "min";
        else if constexpr (std::is_same<Duration, std::chrono::seconds>::value)
            return "s";
        else if constexpr (std::is_same<Duration, std::chrono::milliseconds>::value)
            return "ms";
        else if constexpr (std::is_same<Duration, std::chrono::microseconds>::value)
            return "us";
        else if constexpr (std::is_same<Duration, std::chrono::nanoseconds>::value)
            return "ns";
        else
            return "";
    };
    std::stringstream ss{};
    ss << "laps: " << m_array.size() << ", "
       << "last: " << last<Duration>() << unit() << ", "
       << "avg: " << avg<Duration>() << unit() << ", "
       << "median: " << median<Duration>() << unit() << ", "
       << "stdev: " << stdev<Duration>() << unit()
          ;
    return ss.str();
}

inline void Stopwatch::start()
{
    m_timepoint = Clock::now();
    m_sum = {};
    m_array.clear();
    m_tree.clear();
}

inline void Stopwatch::lap()
{
    auto tp = Clock::now();
    m_array.emplace_back(std::pair<Clock::duration, double>(tp - m_timepoint, 0.0));
    m_timepoint = tp;
    m_tree.insert(m_array.back().first);
    m_sum += m_array.back().first;
}

template<typename Duration> inline
int64_t Stopwatch::last() const
{
    if (m_tree.empty())
        return 0;
    return std::chrono::duration_cast<Duration>(m_array.back().first).count();
}

template<typename Duration> inline
int64_t Stopwatch::avg() const
{
    if (m_array.empty())
        return 0;
    return std::chrono::duration_cast<Duration>(m_sum).count() / m_array.size();
}

template<typename Duration> inline
int64_t Stopwatch::median() const
{
    if (m_tree.empty())
        return 0;
    uint64_t sz = m_tree.size();
    uint64_t idx1 = sz >> 1;
    uint64_t idx2 = idx1 - (!(sz & 1) && (sz & ~uint64_t{1}));
    return std::chrono::duration_cast<Duration>(*m_tree.findByOrder(idx1) + *m_tree.findByOrder(idx2)).count() >> 1;
}

template<typename Duration> inline
int64_t Stopwatch::stdev()
{
    if (m_array.empty())
        return 0;
    int64_t avgRep = avg<Clock::duration>();
    std::for_each(std::execution::par_unseq, m_array.begin(), m_array.end(), [&](auto& p) {
            double mp = p.first.count() - avgRep;
            p.second = mp * mp;
        });
    double stdev = std::reduce(std::execution::par_unseq,
                               m_array.begin(), m_array.end(), Pair{}, [](const auto& lhs, const auto& rhs) {
            auto tmp = lhs;
            tmp.second += rhs.second;
            return tmp;
        }).second;
    stdev = std::sqrt(stdev / m_array.size());
    return std::chrono::duration_cast<Duration>(Clock::duration(static_cast<uint64_t>(stdev))).count();
}

inline size_t Stopwatch::size() const
{
    return m_array.size();
}

inline void Stopwatch::clear()
{
    start();
}

} //!profiling
