#include <iostream>
#include <thread>

#include "jp/Stopwatch.hpp"

using namespace std;
using namespace std::chrono;

int main()
{
    jp::Stopwatch w{};

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

    w.start();
    this_thread::sleep_for(0.6s);
    w.stop();

    cout << w.str<milliseconds>() << "\n";
    cout << w.str<milliseconds>(jp::INFO_LAST | jp::INFO_AVG) << "\n";
    cout << w.str<milliseconds>(~jp::INFO_STDEV) << "\n";

    cout << "\nsize   " << w.size()
         << "\nlast   " << w.last()   << '\t' << w.last<milliseconds>()
         << "\navg    " << w.avg()    << '\t' << w.avg<milliseconds>()
         << "\nmedian " << w.median() << '\t' << w.median<milliseconds>()
         << "\nstdev  " << w.stdev()  << '\t' << w.stdev<milliseconds>()
         << "\n";

    return 0;
}
