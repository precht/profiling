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
    w.stop();
    w.start();
    this_thread::sleep_for(0.5s);
    w.stop();
    w.start();
    this_thread::sleep_for(0.75s);
    w.stop();

    cout << w.str() << "\n";
    cout << w.fullStr() << "\n\n";

    w.start();
    this_thread::sleep_for(0.6s);
    w.stop();

    cout << w.str<chrono::milliseconds>() << "\n";
    cout << w.fullStr<chrono::milliseconds>() << "\n";

    cout << "\nsize   " << w.size()
         << "\nlast   " << w.last()   << '\t' << w.last<chrono::milliseconds>()
         << "\navg    " << w.avg()    << '\t' << w.avg<chrono::milliseconds>()
         << "\nmedian " << w.median() << '\t' << w.median<chrono::milliseconds>()
         << "\nstdev  " << w.stdev()  << '\t' << w.stdev<chrono::milliseconds>()
         << "\n";

    return 0;
}
