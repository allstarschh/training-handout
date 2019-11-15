Welcome to the mandelbrot lab solution!

I hope you are not cheating and have tried your best at optimizing the lab.
The below will spoil the fun by giving away the different hotspots found,
and how to fix them.

# Hardware

The below numbers have been recorded on a Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz
with 16GiB of DIMM DDR3 Synchronous 1600 MHz (0.6 ns) RAM. The lab was compiled
against an optimized build of Qt 5.5.1 with gcc (GCC) 5.3.0 and libstdc++.so.6.0.21.


# Debug vs. Release

This shows why you always should enable compiler optimizations:

qmake CONFIG+=debug && make
perf stat -r 5 ./lab_mandelbrot -b 10

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

      10685.101614      task-clock (msec)         #    1.000 CPUs utilized            ( +-  0.61% )
               392      context-switches          #    0.037 K/sec                    ( +-  3.39% )
                19      cpu-migrations            #    0.002 K/sec                    ( +- 27.60% )
             4,139      page-faults               #    0.387 K/sec                    ( +-  0.45% )
    41,195,546,496      cycles                    #    3.855 GHz                      ( +-  0.52% )
    78,988,076,132      instructions              #    1.92  insns per cycle          ( +-  0.00% )
    11,283,716,173      branches                  # 1056.023 M/sec                    ( +-  0.00% )
        12,399,412      branch-misses             #    0.11% of all branches          ( +-  0.32% )

      10.683737340 seconds time elapsed                                          ( +-  0.61% )

qmake CONFIG+=release && make -B
perf stat -r 5 ./lab_mandelbrot -b 10

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

       3989.280021      task-clock (msec)         #    1.000 CPUs utilized            ( +-  0.19% )
               372      context-switches          #    0.093 K/sec                    ( +-  0.73% )
                12      cpu-migrations            #    0.003 K/sec                    ( +-  8.36% )
             3,109      page-faults               #    0.779 K/sec                    ( +-  3.44% )
    15,307,884,139      cycles                    #    3.837 GHz                      ( +-  0.21% )
    32,274,200,564      instructions              #    2.11  insns per cycle          ( +-  0.00% )
     5,864,150,491      branches                  # 1469.977 M/sec                    ( +-  0.00% )
         8,268,193      branch-misses             #    0.14% of all branches          ( +-  0.26% )

       3.990979568 seconds time elapsed                                          ( +-  0.18% )

So a factor of 2.7x improvement just by enabling compiler optimizations! Not bad at all.
The last value will be our baseline which we will compare later values against.

# __hypot_finit? hypot? cabs?

Profiling the lab as-is shows this interesting hotspot:

    55.44%  lab_mandelbrot   libm-2.22.so                [.] __hypot_finite
             |
             |--54.26%--__hypot_finite
             |          hypot
             |          main
             |
              --1.17%--cabs@plt
                        main

Our code does not call `hypot` anywhere directly, so where does it come from?
This is a result of the optimizer inlining code as good as possible. This
is good for runtime (see above), but not so nice for our profiling problem.

You have two options now: Get a debug build and look at the non-inlined stack
there, or read the annotated assembly from within `perf report`, you can enable
this view by pressing the 'a' key.

The annotated assembly of `main` for the release build shows this:

       │                 for (; iterations < max_iterations && std::abs(z) < 2.0; ++iterations) {
  6.70 │     ↑ je     700
       │     _ZSt13__complex_absCd():
       │     #if _GLIBCXX_USE_C99_COMPLEX
       │       inline float
       │       __complex_abs(__complex__ float __z) { return __builtin_cabsf(__z); }
       │
       │       inline double
       │       __complex_abs(__complex__ double __z) { return __builtin_cabs(__z); }
  0.23 │       movsd  0x8(%rsp),%xmm0
  2.50 │       movsd  0x10(%rsp),%xmm1
  5.97 │     → callq  cabs@plt

There is still no direct line to `hypot` however. The debug build backtrace helps
much more in this context:

    25.15%  lab_mandelbrot   libm-2.22.so               [.] __hypot_finite
             |
             |--16.94%--__hypot_finite
             |          |
             |           --14.11%--hypot
             |                     std::__complex_abs
             |                     std::abs<double>
             |                     drawMandelbrot

Note that you could also use a release build with disabled inlining. So we found
the first hotspot, in our code, the `std::abs` call in the inner loop:

    for (; iterations < max_iterations && std::abs(z) < 2.0; ++iterations) {

We could replace that with `std::norm(z) < 4.0`. This should be much faster, no?
See also the documentation: http://en.cppreference.com/w/cpp/numeric/complex/norm

Let's change it and compare to our baseline:

    for (; iterations < max_iterations && std::norm(z) < 4.0; ++iterations) {

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

       4059.490205      task-clock (msec)         #    1.000 CPUs utilized            ( +-  1.17% )
               375      context-switches          #    0.092 K/sec                    ( +-  2.43% )
                17      cpu-migrations            #    0.004 K/sec                    ( +- 24.28% )
             3,198      page-faults               #    0.788 K/sec                    ( +-  2.68% )
    15,550,922,453      cycles                    #    3.831 GHz                      ( +-  0.69% )
    32,469,935,960      instructions              #    2.09  insns per cycle          ( +-  0.00% )
     5,864,241,192      branches                  # 1444.576 M/sec                    ( +-  0.00% )
         8,311,300      branch-misses             #    0.14% of all branches          ( +-  0.38% )

       4.060600781 seconds time elapsed                                          ( +-  1.18% )

Strange, that is actually slightly worse than our original code! So back to
`perf record` followed by `perf report` (or hotspot) and it turns out we got tricked:

    51.60%  lab_mandelbrot   libm-2.22.so                   [.] __hypot_finite
             |
             |--50.44%--__hypot_finite
             |          hypot
             |          main
             |
              --1.17%--cabs@plt
                        main

At this point, the debug build is again helpful. Alternatively, look at the code
that defines `std::norm` for `std::complex<double>`:

  template <bool>
    struct _Norm_helper
    {
      template <typename _Tp>
        static inline _Tp _S_do_it(const complex<_Tp>& __z)
        {
          const _Tp __x = __z.real();
          const _Tp __y = __z.imag();
          return __x * __x + __y * __y;
        }
    };

  template <>
    struct _Norm_helper<true>
    {
      template <typename _Tp>
        static inline _Tp _S_do_it(const complex<_Tp>& __z)
        {
          _Tp __res = std::abs(__z);
          return __res * __res;
        }
    };

  template <typename _Tp>
    inline _Tp
    norm(const complex<_Tp>& __z)
    {
      return _Norm_helper<__is_floating<_Tp>::__value
            && !_GLIBCXX_FAST_MATH>::_S_do_it(__z);
    }

Ouch! The fast path is only activated when we enabled fast math mode! And the
slow path is worse than just calling `std::abs` as it needs another multiplication.
We have two options now, either we activate it, or we work-around this problem:

    for (; iterations < max_iterations
            && (z.real() * z.real() + z.imag() * z.imag()) < 4.0;
            ++iterations) {

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

       1708.353259      task-clock (msec)         #    0.998 CPUs utilized            ( +-  0.41% )
               346      context-switches          #    0.202 K/sec                    ( +-  0.27% )
                 7      cpu-migrations            #    0.004 K/sec                    ( +- 15.47% )
             2,854      page-faults               #    0.002 M/sec                    ( +-  0.09% )
     6,625,355,531      cycles                    #    3.878 GHz                      ( +-  0.44% )
    18,022,452,261      instructions              #    2.72  insns per cycle          ( +-  0.00% )
     3,100,162,257      branches                  # 1814.708 M/sec                    ( +-  0.00% )
         2,012,581      branch-misses             #    0.06% of all branches          ( +-  0.24% )

       1.711167205 seconds time elapsed                                          ( +-  0.41% )

Much better: An improvement in runtime of 2.3x to our baseline!

# -ffast-math

As an aside, lets compare what happens when we enable -ffast-math here:

Assuming it is safe for the scope of this application, put this into your QMake
.pro file:

QMAKE_CXXFLAGS += -ffast-math

Then rebuild from scratch:

qmake
make -B

With the old code using `std::norm` this gives us:

       1679.568800      task-clock (msec)         #    0.998 CPUs utilized            ( +-  0.42% )
               347      context-switches          #    0.206 K/sec                    ( +-  1.08% )
                 9      cpu-migrations            #    0.006 K/sec                    ( +- 12.85% )
             2,854      page-faults               #    0.002 M/sec                    ( +-  0.11% )
     6,441,346,031      cycles                    #    3.835 GHz                      ( +-  0.29% )
    12,580,112,635      instructions              #    1.95  insns per cycle          ( +-  0.00% )
     2,515,833,091      branches                  # 1497.904 M/sec                    ( +-  0.00% )
         1,957,146      branch-misses             #    0.08% of all branches          ( +-  0.14% )

       1.682811737 seconds time elapsed                                          ( +-  0.42% )

Even better! But let us use the optimized version without fast-math for now.
We'll get back to this again later.

# width, height, loops

Now, if we run perf record + report on our optimized code, we will see the
following hotspots:

             |--57.49%--main
             |
             |--5.74%--_ZNK6QImage5widthEv@plt
             |          |
             |           --3.15%--main
             |
             |--2.50%--QImage::width
             |
             |--2.32%--_ZNK6QImage6heightEv@plt
             |          |
             |           --0.15%--main
             |
             |--2.26%--QImage::height

Looking at the code I wrote, it should be apparent what happens: The
QImage is passed as a pointer to the function. The calls to `width()`
and `height()` in the loops can thus not be hoisted out by the compiler,
as it must assume their value may change at any point. We need to help
the compiler!

    const int width = image->width();
    const int height = image->height();

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            std::complex<double> z(0);
            unsigned int iterations = 0;
            for (; iterations < max_iterations && (z.real() * z.real() + z.imag() * z.imag()) < 4.0; ++iterations) {
                const double cx = cxmin + x * (cxmax - cxmin) / (width - 1);
                const double cy = cymin + y * (cymax - cymin) / (height - 1);

Recompile and run `perf stat` again:

       1276.719766      task-clock (msec)         #    0.997 CPUs utilized            ( +-  0.56% )
               351      context-switches          #    0.275 K/sec                    ( +-  0.36% )
                11      cpu-migrations            #    0.008 K/sec                    ( +- 20.37% )
             2,849      page-faults               #    0.002 M/sec                    ( +-  0.17% )
     4,914,421,055      cycles                    #    3.849 GHz                      ( +-  0.27% )
     9,621,072,141      instructions              #    1.96  insns per cycle          ( +-  0.00% )
     1,514,433,020      branches                  # 1186.191 M/sec                    ( +-  0.00% )
         1,861,869      branch-misses             #    0.12% of all branches          ( +-  0.21% )

       1.280221433 seconds time elapsed                                          ( +-  0.57% )

Not bad, a factor of 1.3x to our previous version without -ffast-math!

But we can help the compiler even more, no? Let us take more code out of the loops:

    const double x_scale = (cxmax - cxmin) / (width - 1);
    const double y_scale = (cymax - cymin) / (height - 1);

    for (int x = 0; x < width; ++x) {
        const double cx = cxmin + x * x_scale;
        for (int y = 0; y < height; ++y) {
            const double cy = cymin + y * y_scale;
            const std::complex<double> c(cx, cy);

            std::complex<double> z(0);
            unsigned int iterations = 0;
            for (; iterations < max_iterations && (z.real() * z.real() + z.imag() * z.imag()) < 4.0; ++iterations) {
                z = z*z + c;
            }

Rerunning perf stat shows that this actually does not change the runtime significantly,
potentially even slows the application down:

       1342.460495      task-clock (msec)         #    0.997 CPUs utilized            ( +-  0.36% )
               348      context-switches          #    0.259 K/sec                    ( +-  0.82% )
                13      cpu-migrations            #    0.010 K/sec                    ( +- 20.06% )
             2,857      page-faults               #    0.002 M/sec                    ( +-  0.09% )
     4,892,611,642      cycles                    #    3.645 GHz                      ( +-  0.20% )
     9,563,872,410      instructions              #    1.95  insns per cycle          ( +-  0.00% )
     1,514,451,976      branches                  # 1128.117 M/sec                    ( +-  0.00% )
         1,869,568      branch-misses             #    0.12% of all branches          ( +-  0.31% )

       1.346049500 seconds time elapsed                                          ( +-  0.35% )

While less instructions are executed, it takes slightly longer to finish, probably because
less instructions are executed per cycle... We will leave it at that and go on to

# color lookup table

Rerunning perf we see that the color calculation takes up a significant amount
of time. We can optimize that by introducing a lookup table (see mandelbrot.cpp
in sol_mandelbrot for the code). Speed improvment?

       1156.042149      task-clock (msec)         #    0.997 CPUs utilized            ( +-  0.62% )
               348      context-switches          #    0.301 K/sec                    ( +-  1.67% )
                 8      cpu-migrations            #    0.007 K/sec                    ( +- 22.46% )
             2,857      page-faults               #    0.002 M/sec                    ( +-  0.11% )
     4,439,901,777      cycles                    #    3.841 GHz                      ( +-  0.22% )
     8,756,945,234      instructions              #    1.97  insns per cycle          ( +-  0.00% )
     1,342,407,573      branches                  # 1161.210 M/sec                    ( +-  0.00% )
         1,767,605      branch-misses             #    0.13% of all branches          ( +-  0.73% )

       1.159218116 seconds time elapsed                                          ( +-  0.62% )

Nice little speedup, and there is no visual difference either.

# setPixel

Last but not least, we can optimize setting the pixel color slightly by operating
on scanlines instead. To do so, reorder the loops and then use:

    for (int y = 0; y < height; ++y) {
        const double cy = cymin + y * y_scale;
        QRgb* scanLine = reinterpret_cast<QRgb*>(image->scanLine(y));
        for (int x = 0; x < width; ++x) {
            const double cx = cxmin + x * x_scale;
            ...
            scanLine[x] = generateColor(iterations, max_iterations);

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

       1114.111232      task-clock (msec)         #    0.996 CPUs utilized            ( +-  0.34% )
               343      context-switches          #    0.308 K/sec                    ( +-  0.48% )
                 7      cpu-migrations            #    0.006 K/sec                    ( +- 19.34% )
             2,814      page-faults               #    0.003 M/sec                    ( +-  0.85% )
     4,301,451,218      cycles                    #    3.861 GHz                      ( +-  0.18% )
     8,314,748,172      instructions              #    1.93  insns per cycle          ( +-  0.00% )
     1,219,583,936      branches                  # 1094.670 M/sec                    ( +-  0.00% )
         2,046,739      branch-misses             #    0.17% of all branches          ( +-  0.09% )

       1.118767046 seconds time elapsed                                          ( +-  0.31% )

Not bad, but not such a big difference.

TODO: move this down

# __muldc3

Perf now shows us a hotspot in __muldc3.

    56.82%  lab_mandelbrot   lab_mandelbrot                 [.] main
             |
             |--31.44%--main
             |
             |--18.15%--__muldc3
             |
             |--7.14%--__muldc3@plt
             |          |
             |           --4.77%--main

This correlates to the `z*z` multiplication in the inner loop. Apparently, the
compiler is not smart enough to detect that this self-multiplication can be done
quite efficiently as:

    z * z = (real(z)^2 - imag(z)^2) - i * 2 * real(z) * imag(z)

Introducing that manually into our code brings another significant performance gain:

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

        673.192215      task-clock (msec)         #    0.995 CPUs utilized            ( +-  0.15% )
               343      context-switches          #    0.509 K/sec                    ( +-  0.92% )
                 5      cpu-migrations            #    0.007 K/sec                    ( +- 21.30% )
             2,054      page-faults               #    0.003 M/sec                    ( +-  0.09% )
     2,605,164,373      cycles                    #    3.870 GHz                      ( +-  0.07% )
     3,804,858,041      instructions              #    1.46  insns per cycle          ( +-  0.00% )
       434,667,656      branches                  #  645.681 M/sec                    ( +-  0.00% )
         1,910,835      branch-misses             #    0.44% of all branches          ( +-  0.22% )

       0.676352515 seconds time elapsed                                          ( +-  0.18% )

Woha, nice!

# -ffast-math again

Let us get back to `-ffast-math`: The default safe math floating point operations
are often the biggest obstacles for the compiler to auto-vectorize your loops.
So it should be much faster, right? Let us renable it again (see above) at this
point and see what difference it makes:

 Performance counter stats for './lab_mandelbrot -b 10' (5 runs):

        742.217383      task-clock (msec)         #    0.996 CPUs utilized            ( +-  0.11% )
               340      context-switches          #    0.458 K/sec                    ( +-  0.50% )
                 6      cpu-migrations            #    0.009 K/sec                    ( +- 26.88% )
             2,053      page-faults               #    0.003 M/sec                    ( +-  0.24% )
     2,874,375,673      cycles                    #    3.873 GHz                      ( +-  0.08% )
     4,186,074,799      instructions              #    1.46  insns per cycle          ( +-  0.00% )
       434,691,153      branches                  #  585.666 M/sec                    ( +-  0.00% )
         1,901,029      branch-misses             #    0.44% of all branches          ( +-  0.23% )

       0.745096019 seconds time elapsed                                          ( +-  0.13% )

Interestingly, the performance actually *decreases* now, for no good reason. I have
not figured out why. Anyhow, to sum this lab up, let look at the initial numbers
we started with, i.e. our baseline:

       3989.280021      task-clock (msec)         #    1.000 CPUs utilized            ( +-  0.19% )
               372      context-switches          #    0.093 K/sec                    ( +-  0.73% )
                12      cpu-migrations            #    0.003 K/sec                    ( +-  8.36% )
             3,109      page-faults               #    0.779 K/sec                    ( +-  3.44% )
    15,307,884,139      cycles                    #    3.837 GHz                      ( +-  0.21% )
    32,274,200,564      instructions              #    2.11  insns per cycle          ( +-  0.00% )
     5,864,150,491      branches                  # 1469.977 M/sec                    ( +-  0.00% )
         8,268,193      branch-misses             #    0.14% of all branches          ( +-  0.26% )

       3.990979568 seconds time elapsed                                          ( +-  0.18% )

Our fastest code instead showed this behavior:

        673.192215      task-clock (msec)         #    0.995 CPUs utilized            ( +-  0.15% )
               343      context-switches          #    0.509 K/sec                    ( +-  0.92% )
                 5      cpu-migrations            #    0.007 K/sec                    ( +- 21.30% )
             2,054      page-faults               #    0.003 M/sec                    ( +-  0.09% )
     2,605,164,373      cycles                    #    3.870 GHz                      ( +-  0.07% )
     3,804,858,041      instructions              #    1.46  insns per cycle          ( +-  0.00% )
       434,667,656      branches                  #  645.681 M/sec                    ( +-  0.00% )
         1,910,835      branch-misses             #    0.44% of all branches          ( +-  0.22% )

       0.676352515 seconds time elapsed                                          ( +-  0.18% )

Step by step we brought down the runtime by a factor of 5.9x. Total number of instructions
executed got reduced by a factor of 8.5x. To further speed it up, we will have to increase
the number of instructions executed per cycle. This is hard work, and will require inspection
of the assembly to remove stalls. One should also look at auto-vectorization here in the future
to make verify that SIMD is used everywhere and that the compiler can vectorize the loop nicely.
