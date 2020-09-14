# profiling
Header-only C++ utilities for measuring performance.

This repo depends on *jp::ordered_set.hpp* 
```bash
git clone https://github.com/precht/profiling.git
wget https://raw.githubusercontent.com/precht/ordered_set/master/include/jp/ordered_set.hpp -P ./profiling/include/jp/
```

## Stopwatch

As with a traditional stopwatch, we remember lap times and we can calculate basic statistics: sum, average, median, and standard deviation.

Time complexity: 
- start(): O(1)
- stop(): O(ln(N))
- sum(): O(1)
- avg(): O(1)
- median(): O(ln(N))
- stdev(): O(N)
- str(): depends on the selected information

Usage:

``` C++
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
```

Possible output:

``` bash
laps: 3, sum: 2'450'430'152ns, last: 750'143'673ns, avg: 816'810'050ns, median: 750'143'673ns, stdev: 289'650'825ns
laps: 4, sum: 3'050ms, last: 600ms, avg: 762ms, median: 675ms, stdev: 267ms
laps: 4, last: 600ms, avg: 762ms
laps: 4, sum: 3'050ms, last: 600ms, avg: 762ms, median: 675ms

size   4
last   600138220	600
avg    762642093	762
median 675140946	675
stdev  267816547	267
```
