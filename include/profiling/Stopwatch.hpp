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

#include <cmath>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <type_traits>

#include "RedBlackTree.hpp"

namespace profiling {

/////////////////////////////////////////////////////////////////////////////// DECLARATION ////////////////////////////

using Info = uint8_t;
constexpr Info INFO_ALL    = 0b1111;
constexpr Info INFO_LAST   = 0b1000;
constexpr Info INFO_AVG    = 0b0100;
constexpr Info INFO_MEDIAN = 0b0010;
constexpr Info INFO_STDEV  = 0b0001;

class Stopwatch
{
public:
    using Clock = std::chrono::steady_clock;

    void start();

    void stop();

    void clear();

    size_t size() const;

    template<typename Duration = Clock::duration>
    int64_t last() const;

    template<typename Duration = Clock::duration>
    int64_t avg() const;

    template<typename Duration = Clock::duration>
    int64_t median() const;

    template<typename Duration = Clock::duration>
    int64_t stdev();

    template<typename Duration = Clock::duration>
    std::string str(Info info = INFO_ALL);

private:
    template<typename Duration>
    constexpr const char* unit();

    bool m_started{};
    Clock::time_point m_timepoint{};
    Clock::duration m_sum{};
    std::vector<Clock::duration> m_array{};
    RedBlackTree<Clock::duration> m_set{};
};

/////////////////////////////////////////////////////////////////////////////// DEFINITION /////////////////////////////

template<typename Duration> inline
std::string Stopwatch::str(Info info)
{
    std::stringstream ss{};
    ss << "laps: " << m_array.size();
    if (info & INFO_LAST)
       ss << ", last: " << last<Duration>() << unit<Duration>();
    if (info & INFO_AVG)
       ss << ", avg: " << avg<Duration>() << unit<Duration>();
    if (info & INFO_MEDIAN)
       ss << ", median: " << median<Duration>() << unit<Duration>();
    if (info & INFO_STDEV)
       ss << ", stdev: " << stdev<Duration>() << unit<Duration>();
    return ss.str();
}

template<typename Duration> inline
constexpr const char* Stopwatch::unit()
{
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
}

inline void Stopwatch::start()
{
    m_started = true;
    m_timepoint = Clock::now();
}

inline void Stopwatch::stop()
{
    auto tp = Clock::now();

    if (not m_started)
        throw std::logic_error("profiling::Stopwatch: stop() should be called after start()!");
    m_started = false;

    m_array.emplace_back(tp - m_timepoint);
    m_timepoint = tp;
    m_set.insert(m_array.back());
    m_sum += m_array.back();
}

inline void Stopwatch::clear()
{
    m_timepoint = {};
    m_started = {};
    m_sum = {};
    m_array.clear();
    m_set.clear();
}

template<typename Duration> inline
int64_t Stopwatch::last() const
{
    if (m_array.empty())
        return 0;
    return std::chrono::duration_cast<Duration>(m_array.back()).count();
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
    if (m_set.empty())
        return 0;
    uint64_t sz = m_set.size();
    uint64_t idx1 = sz >> 1;
    uint64_t idx2 = idx1 - (!(sz & 1) && (sz & ~uint64_t{1}));
    return std::chrono::duration_cast<Duration>(*m_set.findByOrder(idx1) + *m_set.findByOrder(idx2)).count() >> 1;
}

template<typename Duration> inline
int64_t Stopwatch::stdev()
{
    if (m_array.empty())
        return 0;
    int64_t avgRep = avg<Clock::duration>();
    double stdev{};
    for (const auto& p : m_array) {
        double mp = p.count() - avgRep;
        stdev += mp * mp;
    }
    stdev = std::sqrt(stdev / m_array.size());
    return std::chrono::duration_cast<Duration>(Clock::duration(static_cast<uint64_t>(stdev))).count();
}

inline size_t Stopwatch::size() const
{
    return m_array.size();
}

} //!profiling
