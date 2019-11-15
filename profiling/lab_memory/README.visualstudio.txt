Welcome to the memory lab with Visual Studio!

Here are some suggestions for you to try out:

- The code comes with a "benchmark mode", you can enable it by passing the `-b`
  parameter to the executable, followed by the number of iterations. E.g.:

  path\to\lab_memory -b 10

  When you opened the QMake project in Visual Studio, you can set the command
  line argument via the Project Properties > Debugging > Command Arguments. This
  option is also being used by the profiler.

- Set two breakpoints `Worker::loadInput()`, before and after it calls
  `Worker::parseFile`
- Then collect heap snapshots when the breakpoints are hit
- Compare the two snapshots to measure the memory leak for a single call to
  this function
- Find the memory leak and fix it
