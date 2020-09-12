# profiling
Header-only C++ utilities for measuring performance.

## Stopwatch

As with a traditional stopwatch, we store measured laps and we can calculate basic statistics: average, median and standard deviation. Requires  C++17. 

Time complexity: 
- lap(): O(ln(N))
- last(): O(1)
- avg(): O(1)
- median(): O(ln(N))
- stdev(): O(N)
- str(): O(N)

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

this_thread::sleep_for(10ms);
w.lap();
cout << w.str<chrono::microseconds>() << endl;
cout << w.size() << " "
     << w.last<chrono::microseconds>() << " "
     << w.avg<chrono::microseconds>() << " "
     << w.median<chrono::microseconds>() << " "
     << w.stdev<chrono::microseconds>() << " "
     << endl;

w.clear();
cout << w.str<chrono::seconds>() << endl;
```

Output:

``` bash
laps: 4, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms
laps: 5, last: 12029us, avg: 612524us, median: 600171us, stdev: 384083us
5 12029 612524 600171 384083 
laps: 0, last: 0s, avg: 0s, median: 0s, stdev: 0s
```
