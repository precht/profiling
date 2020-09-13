# profiling
Header-only C++ utilities for measuring performance.

## Stopwatch

As with a traditional stopwatch, we remember lap times and we can calculate basic statistics: average, median and standard deviation.

Time complexity: 
- avg(): O(1)
- median(): O(ln(N))
- stdev(): O(N)
- str(): depends on selected informations

Usage:

``` C++
Stopwatch w{};

w.start();
this_thread::sleep_for(1.2s);
w.stop();
w.start();
this_thread::sleep_for(0.5s);
w.stop();
w.start();
this_thread::sleep_for(0.75s);
w.stop();

cout << w.str() << "\n\n";

w.start();
this_thread::sleep_for(0.6s);
w.stop();

cout << w.str<milliseconds>() << "\n";
cout << w.str<milliseconds>(INFO_LAST | INFO_AVG) << "\n";

cout << "\nsize   " << w.size()
     << "\nlast   " << w.last()   << '\t' << w.last<milliseconds>()
     << "\navg    " << w.avg()    << '\t' << w.avg<milliseconds>()
     << "\nmedian " << w.median() << '\t' << w.median<milliseconds>()
     << "\nstdev  " << w.stdev()  << '\t' << w.stdev<milliseconds>()
     << "\n";
```

Output:

``` bash
laps: 3, last: 750138835ns, avg: 816810290ns, median: 750138835ns, stdev: 289639222ns

laps: 4, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms
laps: 4, last: 600ms, avg: 762ms

size   4
last   600146332	600
avg    762644301	762
median 675142583	675
stdev  267805942	267
```
