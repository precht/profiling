#include <iostream>
#include <thread>

#include "profiling.hpp"

using namespace std::chrono_literals;

int main()
{
    profiling::Stopwatch w{};
    w.start();

    std::this_thread::sleep_for(1.2s);
    w.lap();
    std::this_thread::sleep_for(0.5s);
    w.lap();
    std::this_thread::sleep_for(0.75s);
    w.lap();
    std::this_thread::sleep_for(0.6s);
    w.lap();
    std::cout << w.string<std::chrono::milliseconds>() << std::endl;

    std::this_thread::sleep_for(10ms);
    w.lap();
    std::cout << w.string<std::chrono::milliseconds>() << std::endl;

    w.clear();
    std::cout << w.string<std::chrono::microseconds>() << std::endl;

    return 0;
}
