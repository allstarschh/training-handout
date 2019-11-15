This file contains instructions on how to debug lab_cache using GDB to find the
bug.

The effects of the bug are that when making the window wider, the red sine line
disappears or is drawn with weird artifacts like vertical lines. We will debug
the cause for this.
The yellow and white lines were only added for the purpose of visual debugging
and ignored here.

The red line is drawn at the end of PlotWidget::paintEvent().
Either the bug is in QPainter::drawPolyline(), or wrong data is passed to that
function. Sometimes the application prints:
    QPainterPath::lineTo: Adding point where x or y is NaN or Inf, ignoring call
This would indicate wrong data is passed to QPainter::drawPolyline(). If debug
information and source code of the Qt library is available, and if we aren't
afraid to look into the source code of Qt itself, we could break on a warning
message such as this one, by:
    - Setting a breakpoint in qt_message_output
    - Exporting QT_FATAL_WARNINGS=1
Let's instead first verify that wrong data is passed to drawPolyline(),
so let's look at the data:
    (gdb) set max-value-size unlimited
    (gdb) set print elements 0
    (gdb) break plotwidget.cpp:61
    Breakpoint 1 at 0x4037e2: file plotwidget.cpp, line 61.
    (gdb) command 1
    Type commands for breakpoint(s) 1, one per line.
    End with a line saying just "end".
    >print *pointsCached.data()@numPoints
    # Alternative, when having the Qt GDB pretty printers installed:
    # (gdb) print pointsCached
    >continue
    >end

    (gdb) run

    # Make the window wider to trigger the bug
    ...
    }, {xp = 1124.2725830078125, yp = 200}, {xp = 1124.37255859375, yp = 200}, {xp = 1124.4725341796875, yp = 200}, {xp = 1124.572509765625, yp = 200}, {xp = 1124.6724853515625
    , yp = 200}, {xp = 1124.7724609375, yp = 200}, {xp = 1124.8724365234375, yp = 2.7200247563047825e+25}, {xp = 1124.972412109375, yp = 2.7200247563047825e+25}, {xp = 1125.072
    3876953125, yp = 2.7200247563047825e+25}, {xp = 1125.17236328125, yp = 2.7200247563047825e+25}, {xp = 1125.2723388671875, yp = 200}, {xp = 1125.372314453125, yp = 200}, {xp
     = 1125.4722900390625, yp = 200}, {xp = 1125.572265625, yp = 200}, {xp = 1125.6722412109375, yp = 200}, {xp = 1125.772216796875, yp = 200}}

Here we can see an invalid y value of 2.7200247563047825e+25. This confirms
that the bug is passing incorrect data to QPainter::drawPolyline().
The code that fills pointsCached is:
    const float cached = cachedSin(x * omega);
    pointsCached[i] = mapPoint(x, cached);
The incorrect value can either originate from a buggy "mapPoint" function or an
incorrect value of "cached" from the "cachedSin" function. Let's find out:
    (gdb) delete breakpoint 1
    (gdb) dprintf plotwidget.cpp:46,"i: %d, original: %f, cached: %f, diff: %f\n", i, original, cached, original - cached
    Dprintf 2 at 0x40355a: file plotwidget.cpp, line 46.
    (gdb) continue

    # Change size of window again to trigger repaint

    i: 9712, original: 0.438387, cached: 0.000000, diff: 0.438387
    i: 9713, original: 0.436975, cached: 272002483556813597835264.000000, diff: -272002483556813597835264.000000

This confirms that "cached" itself is buggy. To see what is happening, let's
place a breakpoint and step into cachedSin:
    (gdb) break plotwidget.cpp:46
    Breakpoint 3 at 0x40355a: file plotwidget.cpp, line 46.
    (gdb) condition 3 (original - cached > 1) || (original - cached < -1)
    (gdb) continue

Once the breakpoint is hit, reverse debugging into the previous cachedSin()
call would be useful! However, we step into the next cachedSin() call and hope
that still triggers the bug:
    # Change size of window again to trigger repaint
    (gdb) p original
    $1 = 0.0880945027
    (gdb) p cached
    $2 = -2.62329398e+33
    (gdb) n
    (gdb) n
    (gdb) n
    (gdb) n
    (gdb) n
    (gdb) s
    CachedSin::operator() (this=0x6061c0 <PlotWidget::paintEvent(QPaintEvent*)::cachedSin>, x=6.36668158) at cachedsin.cpp:23
    (gdb) n
    (gdb) p index
    $3 = 1014
    (gdb) p x
    $4 = 6.37296534
    (gdb) p m_values[index]
    $5 = -2.62329398e+33

This shows that the sine lookup table at index 1014 contains an invalid entry.
We can find the source of the problem right away:
    (gdb) p cachedSin
    $6 = {m_values = 0x6a6670, m_numValues = 1000, m_deltaX = 0.00628318544}
Here we see that m_numValues is 1000, therefore an access at index 1014 is an
out-of-bounds access, which produces undefined behaviour and in our case random
values from uninitialized memory.

Even if we wouldn't have seen m_numValues or if the member variable didn't exist,
we can still find the bug by checking where the invalid entry at index 1014
originated from: Either the lookup table contained an invalid entry right from
the beginning, or it was overwritten with an invalid entry later. Watchpoints
are useful for the second case. Let's check the first case and see if the lookup
table contains the correct values right from the start, by simply printing all
values upon construction:
    (gdb) kill
    Kill the program being debugged? (y or n) y
    (gdb) dprintf cachedsin.cpp:12,"index: %d value: %f\n",i-1,m_values[i-1]
    Dprintf 4 at 0x403134: file cachedsin.cpp, line 12.
    (gdb) delete breakpoint 2
    (gdb) delete breakpoint 3
    (gdb) run
    ...
    index: 996 value: -0.025069
    index: 997 value: -0.018787
    index: 998 value: -0.012505

Now one can see that the lookup table has only 1000 entries. Accessing element 1014
therefore accesses the array out of its bounds.

We can fix the out-of-bounds access by replacing
    int index = static_cast<int>(x / m_deltaX);
with
    int index = static_cast<std::size_t>(x / m_deltaX) % m_numValues;
Since a sine repeats itself, this even produces correct results when looking up
values outside of the lookup table size.

This example is somewhat contrived to show this issue. If one would use a proper
container like std::vector, one would get an assertion when extra debug checks
are enabled (like _GLIBCXX_DEBUG for GCC's libstdc++). Other libraries like
Qt's QVector also have out-of-bounds checking with asserts.
