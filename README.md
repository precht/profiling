# profiling
Header-only C++ utilities for measuring performance.

## Stopwatch

As with a traditional stopwatch, we remember lap times and we can calculate basic statistics: average, median and standard deviation.

Time complexity: 
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
```

Output:

``` bash
laps: 3, last: 750399901ns, avg: 816909894ns, median: 750399901ns
laps: 3, last: 750399901ns, avg: 816909894ns, median: 750399901ns, stdev: 289629260ns

laps: 4, last: 600ms, avg: 762ms, median: 675ms
laps: 4, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms

size   4
last   600087205	600
avg    762704222	762
median 675243553	675
stdev  267821948	267
```
