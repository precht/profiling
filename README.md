# profiling
Header-only C++ utilities for measuring performance.

## Stopwatch

As with a traditional stopwatch, we remember lap times and we can calculate basic statistics: average, median and standard deviation.

Time complexity: 
- lap(): O(ln(N))
- last(): O(1)
- avg(): O(1)
- median(): O(ln(N))
- stdev(): O(N)
- str(): O(ln(N))
- fullStr(): O(N)

Usage:

``` C++
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
```

Output:

``` bash
laps: 3, last: 750132391ns, avg: 816793588ns, median: 750132391ns
laps: 3, last: 750132391ns, avg: 816793588ns, median: 750132391ns, stdev: 289608715ns
laps: 4, last: 600ms, avg: 762ms, median: 675ms
laps: 4, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms
laps: 0, last: 0ns, avg: 0ns, median: 0ns

size: 1
last: 5669 5
avg: 5669 5
median: 5669 5
stdev: 0 0
```
