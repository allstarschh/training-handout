Welcome to the concurrent mandelbrot lab!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  .\lab_mandelbrot_concurrent -b 5

- Make sure you compile the code in release mode!

- To measure a baseline for how the code performs initially before you start
  optimizing it, run the above command while you have tracing enabled in UIforETW.

- Analyze the result file in WPA and investigate how many CPU cores are used by
  the lab code.

- Use the CPU Usage (Precise) column to investigate lock contention.

- Find bottlenecks, change the code, rinse repeat and always make sure to check
  against the baseline to verify that your change brings a significant performance
  win.
