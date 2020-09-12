# profiling
Header-only C++ utilities for measuring performance.

## Stopwatch

As with a traditional stopwatch, we store measured laps and we can calculate basic statistics: average, median and standard deviation. Requires  C++17. 

Time complexity: 

- lap(): O(ln(N))
- avg(): O(1)
- median(): O(ln(N))
- stdev(): O(N)
- string(): O(N)

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
cout << w.string<chrono::milliseconds>() << endl;

this_thread::sleep_for(10ms);
w.lap();
cout << w.string<chrono::milliseconds>() << endl;

w.clear();
cout << w.string<chrono::microseconds>() << endl;
```

Output:

``` bash
laps: 4, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms
laps: 5, last: 12ms, avg: 612ms, median: 600ms, stdev: 383ms
laps: 0, last: 0us, avg: 0us, median: 0us, stdev: 0us
```
