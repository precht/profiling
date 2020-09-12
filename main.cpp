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

    cout << w.str() << '\n';
    cout << w.fullStr() << '\n';

    this_thread::sleep_for(0.6s);
    w.lap();

    cout << w.str<chrono::milliseconds>() << '\n';
    cout << w.fullStr<chrono::milliseconds>() << '\n';

    w.start();
    cout << w.str() << '\n';

    w.lap();
    cout << "\nsize: "   << w.size()
         << "\nlast: "   << w.last()   << ' ' << w.last<chrono::microseconds>()
         << "\navg: "    << w.avg()    << ' ' << w.avg<chrono::microseconds>()
         << "\nmedian: " << w.median() << ' ' << w.median<chrono::microseconds>()
         << "\nstdev: "  << w.stdev()  << ' ' << w.stdev<chrono::microseconds>()
         << endl;

    return 0;
}
