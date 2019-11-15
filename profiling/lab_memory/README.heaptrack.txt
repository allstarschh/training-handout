Welcome to the memory lab!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  ./lab_memory --benchmark 5

- Use one of the following tools to evaluate the memory consumption of the lab:

  /usr/bin/time -v
  valgrind --tool=massif
  heaptrack

  Also measure the baseline runtime performance with e.g. `perf stat`:

  perf stat -r 5 ./lab_memory --benchmark 5

  It is advised that you repeat these steps whenever you change the code. Use it
  to verify that your change brings a significant performance win, either to
  reduce the memory consumption, or the improve the runtime performance.

- Use heaptrack to find issues with the heap usage of the lab and improve the
  code. Then repeat the measurements above to verify the impact of your changes.

- Once you have collected a baseline file with heaptrack, you can compare later
  data sets to that baseline to check the influence of your patches:

  heaptrack_gui -b heaptrack.BASELINE.gz heaptrack.NEWVERSION.gz
