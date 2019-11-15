Welcome to the concurrent mandelbrot lab solution!

I hope you are not cheating and have tried your best at optimizing the lab.
The below will spoil the fun by giving away the different hotspots found,
and how to fix them.

# Hardware

The below numbers have been recorded on a Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz
with 16GiB of DIMM DDR3 Synchronous 1600 MHz (0.6 ns) RAM. The lab was compiled
against an optimized build of Qt 5.5.1 with gcc (GCC) 5.3.0 and libstdc++.so.6.0.21.

# baseline

First, let us create a baseline for the original lab code:

$ perf stat -o stat.baseline.txt -r 10 ./lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5

 Performance counter stats for './lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5' (10 runs):

       8446.962563      task-clock (msec)         #    1.003 CPUs utilized       ( +-  0.42% )
             6,233      context-switches          #    0.738 K/sec               ( +-  0.13% )
               510      cpu-migrations            #    0.060 K/sec               ( +-  6.22% )
             3,990      page-faults               #    0.472 K/sec               ( +-  3.84% )
    32,282,248,765      cycles                    #    3.822 GHz                 ( +-  0.10% )
    59,020,150,192      instructions              #    1.83  insn per cycle      ( +-  0.01% )
     6,666,871,684      branches                  #  789.263 M/sec               ( +-  0.01% )
        10,531,368      branch-misses             #    0.16% of all branches     ( +-  0.12% )

       8.419625514 seconds time elapsed                                          ( +-  0.42% )

We already see a big problem here in the very first line output by perf stat:
It tells us that only one CPU is really utilized, even though the code spins
up multiple threads!

Let us also look at the information we gather from `perf trace`:

$ perf trace -o trace.baseline.txt -s ./lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5

If you open the resulting `trace.baseline.txt` file in an editor, you'll see an
aggregated list of syscalls with the number of calls and how long it took
to complete the different calls. Here are the interesting calls on my machine:

Summary of events:

 lab_mandelbrot_ (22597), 9240 events, 25.7%, 0.000 msec

   syscall            calls    total       min       avg       max      stddev
                               (msec)    (msec)    (msec)    (msec)        (%)
   --------------- -------- --------- --------- --------- ---------     ------
   ...
   clone                162    37.054     0.200     0.229     0.434      1.02%
   ...
   futex               1080  7318.923     0.001     6.777  1456.600     43.97%
   ...

The `clone` syscall is used to create new threads. Apparently the lab creates 162
threads on an 8 core machine. This is a severe case of overcommitting, which
can degenerate performance, see below.

But, even worse, the second line shows that we spend 7.3s inside the futex
(fast userspace mutex) syscall. That indicates we suffer immensely from lock
contention.

# Reducing the Critical Section

NOTE: If you have a VTune license available, check out their Locks & Waits analysis!
It is much better than what you get from perf currently.

With perf we can profile the sleep time to find which mutex is heavily contended:

$ perf-sleep-record ./lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent
$ perf report --sort comm --show-total-period --no-children

[or for a GUI solution, use hotspot's off-cpu profiling. perf report is just easier to paste here]

#
# Overhead        Period  Command
# ........  ............  ...............
#
    91.59%   2521275729576  MandelBrotRowTh
            |
            ---__schedule
               schedule
               futex_wait_queue_me
               futex_wait
               do_futex
               sys_futex
               entry_SYSCALL_64_fastpath
               |
                --91.59%--syscall
                          |
                           --91.18%--QBasicMutex::lockInternal
                                     (anonymous namespace)::MandelBrotRowThread::run

Ouch! The total period in which code was sleeping was a whopping ~250s! Note
that the period printed by perf has the strange unit of 0.1ns. The mutex
is inside our mandel brot calculation (no big suprise here). Investigating the
code, it should become clear that every call to firstPassMandelbrotRow
operates on distinct pixel lines. Thus there is no need to guard that
part with a mutex - only the histogram and total numbers must be guarded.

Thus we move the mutex locker down into loop, after the expensive mandelbrot
calculation, recompile and tada:

 Performance counter stats for './lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5' (10 runs):

      10751.742723      task-clock (msec)         #    4.245 CPUs utilized            ( +-  0.14% )
           136,970      context-switches          #    0.013 M/sec                    ( +-  1.60% )
            28,012      cpu-migrations            #    0.003 M/sec                    ( +-  2.41% )
             4,311      page-faults               #    0.401 K/sec                    ( +-  2.53% )
    39,604,717,110      cycles                    #    3.684 GHz                      ( +-  0.15% )
    60,916,978,155      instructions              #    1.54  insn per cycle           ( +-  0.04% )
     7,019,307,327      branches                  #  652.853 M/sec                    ( +-  0.06% )
        15,660,318      branch-misses             #    0.22% of all branches          ( +-  0.34% )

       2.533032345 seconds time elapsed                                               ( +-  0.31% )

By reducing lock contention we greatly increased the performance of this lab.
Now, 4.6 of my 8 cores are utilized, and the runtime reduces by a factor of
3.4. All of that, just by moving a mutex lock to reduce the critical section.

# Locking

Let us take another look at the locking in the code. We will notice that we use
a mutex just to protect integer increments. We can do better than that by
leveraging atomic integer operations in the form of QAtomicInteger. Refactoring
the code to remove the mutex brings another small performance gain:

$ perf stat -r 10 ./lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5

 Performance counter stats for './lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5' (10 runs):

       9929.203460      task-clock (msec)         #    4.191 CPUs utilized            ( +-  0.09% )
             3,200      context-switches          #    0.322 K/sec                    ( +-  2.51% )
               580      cpu-migrations            #    0.058 K/sec                    ( +-  4.66% )
             3,786      page-faults               #    0.381 K/sec                    ( +-  2.05% )
    36,833,679,899      cycles                    #    3.710 GHz                      ( +-  0.09% )
    61,115,322,996      instructions              #    1.66  insn per cycle           ( +-  0.00% )
     6,662,185,471      branches                  #  670.969 M/sec                    ( +-  0.00% )
        10,041,106      branch-misses             #    0.15% of all branches          ( +-  0.11% )

       2.369107475 seconds time elapsed                                               ( +-  0.36% )

But do note how atomic operations are no magic weapon that will make all parallel code
significantly faster. Futex is well optimized and, if uncontended, has only a very
small overhead. So before rewriting your code to use complex atomics, first try to
reduce the size of your critical sections.

# More parallelization

Now, looking at the results above we note that not all of my 8 cores are utilized,
only ~4.2 of them. So, why is that? Looking at the thread graph below the flamegraph
in hotspot shows it clearly: the reason is that we so far only parallelize the
first pass of the mandelbrot algorithm.
The second pass required for this mandelbrot image calculation is currently done
sequentially. Amdahl's law tells us that any sequential code severely limits the
potential gain from parallelizing another part. Thankfully, with QtConcurrent,
it is very easy to parallelize this step as well, bringing us to the final numbers:

 Performance counter stats for './lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5' (10 runs):

      11307.378264      task-clock (msec)         #    7.611 CPUs utilized            ( +-  0.14% )
             3,936      context-switches          #    0.348 K/sec                    ( +-  1.03% )
               670      cpu-migrations            #    0.059 K/sec                    ( +-  2.01% )
             3,856      page-faults               #    0.341 K/sec                    ( +-  3.53% )
    41,725,106,677      cycles                    #    3.690 GHz                      ( +-  0.14% )
    61,153,654,159      instructions              #    1.47  insn per cycle           ( +-  0.00% )
     6,666,461,570      branches                  #  589.567 M/sec                    ( +-  0.00% )
        10,012,238      branch-misses             #    0.15% of all branches          ( +-  0.12% )

       1.485680918 seconds time elapsed                                               ( +-  0.28% )

This change has a huge impact on the performance! The reason is simple: We give
our cores more work to do in parallel. Now finally we get the CPU utilization close
to the goal of ~8 for my machine.

# Overcommitting

Finally, let us get back to the issue we noticed initially, namely that we create
162 threads even though my machine has only 8 cores available.

To fix this, simply swap out the hard-coded number of 32 max threads of the
threadpool by a call to `QThread::idealThreadCount`, which will fix the issue
on my machine and also make the code scale better on beefy machines with more
than 32 cores:

Repeating the `perf trace` steps above, we now see:

   syscall            calls    total       min       avg       max      stddev
                               (msec)    (msec)    (msec)    (msec)        (%)
   --------------- -------- --------- --------- --------- ---------     ------
   clone                 42     9.909     0.204     0.236     0.316      1.83%

Much better! This change also considerably reduces the memory requirements
for the lab, as we no longer need to reserve stack space for the other threads:

Before:
/usr/bin/time -v ./lab_mandelbrot_concurrent/lab_mandelbrot_concurrent -b 5  |& grep Max
        Maximum resident set size (kbytes): 53588

After:
/usr/bin/time -v ./lab_mandelbrot_concurrent/lab_mandelbrot_concurrent -b 5  |& grep Max
        Maximum resident set size (kbytes): 46860

Note though that the runtime of the lab does not significantly change on my
machine at least, YMMV:

 Performance counter stats for './lab_mandelbrot_concurrent/src/lab_mandelbrot_concurrent -b 5' (10 runs):

      11311.410164      task-clock (msec)         #    7.627 CPUs utilized            ( +-  0.10% )
            36,793      context-switches          #    0.003 M/sec                    ( +-  1.30% )
             4,725      cpu-migrations            #    0.418 K/sec                    ( +-  1.44% )
             3,521      page-faults               #    0.311 K/sec                    ( +-  2.39% )
    41,741,512,583      cycles                    #    3.690 GHz                      ( +-  0.10% )
    61,284,128,037      instructions              #    1.47  insn per cycle           ( +-  0.00% )
     6,695,615,348      branches                  #  591.935 M/sec                    ( +-  0.01% )
        10,370,406      branch-misses             #    0.15% of all branches          ( +-  0.15% )

       1.483027172 seconds time elapsed                                          ( +-  0.28% )

Note that this change has a much more observable effect in VTune's concurrency
or locks & waits analysis, as the number of threads goes down and the occupy
the CPU more evenly. Also the summary page there will tell us that we now
better utilize the ideal number of threads.

An even better approach is to remove the custom pool completely and instead
reuse the global QThreadPool. This removes the thread creation overhead when
calling drawMandelbrot multiple times.

# Summary

Overall, we decreased the runtime of the lab from 8.4s down to 1.5s, i.e. by
a factor of 5.6. Instead of only utilizing one core, we now properly leverage
up to 8 cores. If we use taskset to look at the scaling behavior, the results
are very pleasing as well:

taskset -c 1 perf stat ./lab_mandelbrot_concurrent/lab_mandelbrot_concurrent -b 5
        8488.520760      task-clock (msec)         #    1.000 CPUs utilized
        8.491534272 seconds time elapsed
        != 100%
taskset -c 1-2 perf stat ./lab_mandelbrot_concurrent/lab_mandelbrot_concurrent -b 5
        8580.581186      task-clock (msec)         #    1.986 CPUs utilized
        4.320650430 seconds time elapsed
        ~50%
taskset -c 1-4 perf stat ./lab_mandelbrot_concurrent/lab_mandelbrot_concurrent -b 5
        8955.055811      task-clock (msec)         #    3.920 CPUs utilized
        2.284390292 seconds time elapsed
        ~27%
taskset -c 1-8 perf stat ./lab_mandelbrot_concurrent/lab_mandelbrot_concurrent -b 5
        10815.226908      task-clock (msec)         #    6.717 CPUs utilized
        1.610076555 seconds time elapsed
        ~19%
