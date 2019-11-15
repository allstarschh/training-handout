Welcome to the mandelbrot lab!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  ./lab_mandelbrot -b 10

- To measure a baseline for how the code performs initially before you start
  optimizing it, use `perf stat` paired with the above, e.g.:

  perf stat -r 5 ./lab_mandelbrot -b 10

  You can also store the result in a file to look it up later for comparison:

  perf stat -o stat.baseline.txt -r 5 ./lab_mandelbrot -b 10

  It is advised that you repeat this step whenever you change the code. Use it
  to verify that your change brings a significant performance win.

- Make sure you compile the code in release mode! It is highly educational to
  compare the performance of a release build and a debug build.

- With `perf record` and `perf report` you can look at where the time is spent
  and find hotspots you can then optimize. Some hints:

  * Enable stack collection via `--call-graph dwarf` or `--call-graph lbr`
  * Look at different hardware counters via `--event ...`, interesting are e.g.
    cycles, instructions, cache-misses
  * Compare the results shown to you for these calls:

    perf report
    perf report --inline
    perf report --inline --no-children
    perf report --inline --no-children -g srcline -s dso,sym,srcline

    Which one do you find the most useful to find hotspots in the code to optimize?

  * You can also look at the annotated assembly of a hotspot from within
    `perf report` by pressing the 'a' key.

- Optionally, you can also generate flame graphs to compare the before and after
  state:

    perf script | stackcollapse-perf | flamegraph > out.svg
    firefox out.svg

- The best tool to visualize all this, however, is :

    hotspot
