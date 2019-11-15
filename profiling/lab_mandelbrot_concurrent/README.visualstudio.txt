Welcome to the concurrent mandelbrot lab!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  path\to\lab_mandelbrot_concurrent -b 10

  When you opened the QMake project in Visual Studio, you can set the command
  line argument via the Project Properties > Debugging > Command Arguments. This
  option is also being used by the profiler.

- Measure a baseline for how the code performs initially before you start
  optimizing it. Either use the Qt TestLib benchmark, use `timemem`, or record
  an initial data set on the above command.

  It is advised that you repeat this step whenever you change the code. Use it
  to verify that your change brings a significant performance win.

- Make sure you compile the code in release mode!

- With the concurrency profiler in Visual Studio you can then investigate the
  lock contention and try to improve the code. Some things to try out:

  * Look at the call tree to find the place where a mutex is contented.
  * Look at the annotated code for the hotspot function.
  * Try to reduce the critical section, then repeat the measurement.
  * Try to use atomics instead of a mutex, how does this influence the runtime?
