#include <iostream>
#include <thread>

#include "profiling/Stopwatch.hpp"

using namespace std;
using namespace std::chrono_literals;

int main()
{
    profiling::Stopwatch w{};
    w.start();

    this_thread::sleep_for(1.2s);
    w.lap();
    this_thread::sleep_for(0.5s);
    w.lap();
    this_thread::sleep_for(0.75s);
    w.lap();
    this_thread::sleep_for(0.6s);
    w.lap();
    cout << w.str<chrono::milliseconds>() << endl;
    cout << w.fullStr<chrono::milliseconds>() << endl;

    this_thread::sleep_for(10ms);
    w.lap();
    cout << w.fullStr<chrono::microseconds>() << endl;
    cout << w.size() << " "
         << w.last<chrono::microseconds>() << " "
         << w.avg<chrono::microseconds>() << " "
         << w.median<chrono::microseconds>() << " "
         << w.stdev<chrono::microseconds>() << " "
         << endl;

    w.start();
    cout << w.str<chrono::seconds>() << endl;

    return 0;
}
