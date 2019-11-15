Welcome to the concurrent mandelbrot lab!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  ./lab_mandelbrot_concurrent -b 5

- To measure a baseline for how the code performs initially before you start
  optimizing it, use `perf stat` paired with the above, e.g.:

  perf stat -r 5 ./lab_mandelbrot_concurrent -b 5

  You can also store the result in a file to look it up later for comparison:

  perf stat -o stat.baseline.txt -r 5 ./lab_mandelbrot_concurrent -b 5

  It is advised that you repeat this step whenever you change the code. Use it
  to verify that your change brings a significant performance win.

- Make sure you compile the code in release mode!

- It is very educational to look at how this code scales to the number of cores
  available.

  nproc # prints the number of cores available
  # let's assume it's 8, now check how the performance changes when we only use 4:
  taskset -c 1-4 perf stat -r 5 ./lab_mandelbrot_concurrent -b 5
  # what about just 2?
  taskset -c 1-2 perf stat -r 5 ./lab_mandelbrot_concurrent -b 5
  # what about just 1?
  taskset 1 perf stat -r 5 ./lab_mandelbrot_concurrent -b 5

- Use perf trace to investigate the syscalls that this lab is executing:

  perf trace -o trace.baseline.txt -s ./lab_mandelbrot_concurrent -b 5

- Use perf (or hotspot's off-cpu profiling) to profile the sleep time in this lab:

  perf-sleep-record ./lab_mandelbrot_concurrent -b 2
  perf-sleep-report

- Find bottlenecks, change the code, rinse repeat and always make sure to check
  against the baseline to verify that your change brings a significant performance
  win.
