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

```

Output:

``` bash
laps: 4, last: 600ms, avg: 762ms, median: 675ms
laps: 4, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms
laps: 5, last: 10330us, avg: 612192us, median: 600173us, stdev: 384620us
5 10330 612192 600173 384620 
laps: 0, last: 0s, avg: 0s, median: 0s
```
