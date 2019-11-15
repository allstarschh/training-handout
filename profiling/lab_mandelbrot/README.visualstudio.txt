Welcome to the mandelbrot lab!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  path\to\lab_mandelbrot -b 10

  When you opened the QMake project in Visual Studio, you can set the command
  line argument via the Project Properties > Debugging > Command Arguments. This
  option is also being used by the profiler.

- Measure a baseline for how the code performs initially before you start
  optimizing it. Either use the Qt TestLib benchmark, or use `timemem` or record
  an initial data set on the above command.

  It is advised that you repeat this step whenever you change the code. Use it
  to verify that your change brings a significant performance win.

- Make sure you compile the code in release mode! But it is highly educational to
  compare the performance of a release build and a debug build.

- With the sampling profiler in Visual Studio you can then investigate the CPU
  hotspots and try to improve the code. Some things to try out:

  * Increase the sampling rate via Performance Explorer > Properties
  * Also try other event sources such as Instruction Counts
  * Look at the annotate code for the hot spot function
