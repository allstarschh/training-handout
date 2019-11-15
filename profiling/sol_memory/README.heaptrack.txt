So how to we go through this lab systematically to improve it?

First of all it's important to measure the baseline performance before changing
any of the code:

$ perf stat --repeat 5 ./lab_memory -b 5

< lots of output omitted >

 Performance counter stats for './lab_memory -b 5' (5 runs):

       1236.935703      task-clock (msec)         #    0.823 CPUs utilized            ( +-  1.15% )
             1,759      context-switches          #    0.001 M/sec                    ( +-  4.05% )
                 3      cpu-migrations            #    0.003 K/sec                    ( +- 36.17% )
           145,337      page-faults               #    0.117 M/sec                    ( +-  0.00% )
     4,637,053,700      cycles                    #    3.749 GHz                      ( +-  1.09% )
     5,013,219,377      instructions              #    1.08  insn per cycle           ( +-  0.17% )
     1,141,979,475      branches                  #  923.233 M/sec                    ( +-  0.15% )
        40,054,971      branch-misses             #    3.51% of all branches          ( +-  0.49% )

       1.502452947 seconds time elapsed                                          ( +-  2.60% )

We also want to ensure that we don't break anything. A quick check for that is
to hash the output of the tool:

$ ./lab_memory -b 5 | md5sum
dd22c01472609f36509e2eb987bf100a  -

OK, now let's use heaptrack to inspect the lab:

$ heaptrack ./lab_memory -b 5
< again lots of output >
heaptrack stats:
        allocations:            2960820
        leaked allocations:     562630
        temporary allocations:  29672

Good, time to fire up the GUI to inspect the data:

$ heaptrack_gui heaptrack.lab_memory.12345.gz

Some things that you should take note of on the summary page:

- total runtime: 3.159s, which is much longer than the perf stat
  number above. This is due to the overhead imposed by heaptrack, so ~2x here
- calls to allocation functions: 2960820 (937264/s)
  this is a lot, and indicates that removing allocations can improve runtime
  performance
- temporary allocations: 34058 (1.15%, 10781/s)
  not too many, this looks OKish right now
- peak heap memory consumption: 584.0 MB after 3.088s
  quite a bit, and it turns out that...
- total memory leaked: 576.1 MB
  we apparently suffer from a memory leak
- the top hotspots on the summary page show you the places that called an
  allocation function like malloc. Only places that contribute to more than
  1% of the respective metric is shown.

We directly see where we seem to be leaking memory here startsWith is to blame.
Hover that entry and you see mailparser.cpp:24, which of course is super ugly
and broken. Fix it by removing all the C code and instead use the following
C++ one-liner: `haystack.compare(0, needle.size(), needle) == 0`.

Recompile and then rerun heaptrack and we are down to:

heaptrack stats:
        allocations:            2398220
        leaked allocations:     30
        temporary allocations:  449667

heaptrack_gui also shows us that the temporary allocation metric went up,
because we removed the leaked allocations in startsWith:

    temporary allocations: 454053 (18.93%, 170376/s)

Now let's switch over to the flamegraph view, which is really the most useful
visualization we have available to us.

- select the "temporary allocations" metric and look at the backtrace
  visualization. Seems like we really need to get string allocations in
  MailParser::parse under control!
- switch to the caller/caleee view and enter "MailParser::parse" in the
  function filter line edit and sort the view by temporary (inclusive) cost
- we now see that mailparser.cpp:78 allocates the most temporary string instances
- similarly the other calls to startsWith also allocate a lot of temporary strings
- right click and select "open in editor"
- turns out we construct the temporary strings because we pass the strings by value
  to startsWith, which copies the long strings (beware of SSO!)
- so fix this, recompile and rerun heaptrack:

heaptrack stats:
        allocations:            2025790
        leaked allocations:     30
        temporary allocations:  170331

Much better! You can use the diffing feature of heaptrack to verify that you
removed a ton of temporary allocations:

heaptrack_gui -b heaptrack.before.gz heaptrack.after.gz

But there are still quite some temporary allocations, right? So let's have another
look at heaptrack_gui and you should notice that mailparser.cpp:78 still allocates
tons of temporary strings. The reason is the long string identifier we pass as
second argument to startsWith. Hoist that out of the loop into a variable and
you'll remove tons more allocations:

heaptrack stats:
        allocations:            1885145
        leaked allocations:     30
        temporary allocations:  29681

Note also how the other strings for From/Date/Subject are small and fit into the
Small String buffer of std::string. They do not trigger any allocations, and thus
are much faster. Still, hoisting that out of the loop can be worthwhile, but
heaptrack won't find these cases - a CPU profiler would tell you if it's a hotspot
or not.

OK, but back to profiling with heaptrack. When we look at the latest data file,
we notice that the temporary allocation hotspot now points to worker.cpp:30:

    for (const std::pair<std::string, int> &wc : mail.wordCount) {

This is nasty! Because the map uses `pair<const string, unsigned>` as internal
type, the above will copy the strings! You can fix this by using auto:

    for (const auto &wc : mail.wordCount) {

New results are:

heaptrack stats:
        allocations:            1858315
        leaked allocations:     30
        temporary allocations:  2851

Going back to heaptrack, we then see worker.cpp:24:

    for (auto mail : parser.parse(filePath)) {

Which of course should be

    for (const auto &mail : parser.parse(filePath)) {

Note how the number of temporary allocations is low, but fixing this hotspot here
dramatically improves total number of allocations. The temporary metric is a good
indication to find easy-to-fix issues, really:

heaptrack stats:
        allocations:            1311940
        leaked allocations:     30
        temporary allocations:  2851

Funnily enough, we removed the one temporary allocation hotspot, just to uncover
another with exactly the same amount of cost ;-) But the overall allocations went
down significantly, so definitely worth it!

Let's switch to the flamegraph view for `Allocations` using the latest heaptrack
data file. It shows us one branch that goes through std::list from
MailParser::parse. The Bottom-Up view shows that this is related to this line
in the code:

            mails.push_back(currentMail);

We are copying the word count map! Let's move instead:

            mails.push_back(std::move(currentMail));

Again, let's run heaptrack to verify that this changes things for the better:

heaptrack stats:
        allocations:            765565
        leaked allocations:     30
        temporary allocations:  2851

Indeed, this significantly improves the situation, excellent!

Now, we could continue looking at the various metrics and improve one after the
other. The solution implements the above and then some:

- use interning, i.e. share strings instead of having them multiple times in
  memory
- use (sorted) vectors instead of std::map to store the data in a more compact
  way, which reduces memory consumption slightly, but makes things (a bit) slower
- hand-roll our own word parser instead of using stringstream to remove more
  allocations
- reduce padding by moving isValid data member in Mail down
- remove padding altogether by not storing isValid and words as variables, but
  instead use functions to query for these values on-demand
- do not use std::list, use std::vector

Overall this brings us to this situation for the solution:

heaptrack stats:
        allocations:            84360
        leaked allocations:     30
        temporary allocations:  3876

peak heap memory consumption: 7.0 MiB after 0.476s

The output is still the same, so we didn't break anything:

$ ./sol_memory -b 5 | md5sum
dd22c01472609f36509e2eb987bf100a  -

But note how the (sorted) vector approach is making things slower:

 Performance counter stats for './sol_memory -b 5' (5 runs):

       1036.804892      task-clock:u (msec)       #    0.710 CPUs utilized            ( +-  1.60% )
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
             2,000      page-faults:u             #    0.002 M/sec                    ( +-  0.10% )
     2,086,361,729      cycles:u                  #    2.012 GHz                      ( +-  1.09% )
     3,776,032,780      instructions:u            #    1.81  insn per cycle           ( +-  0.00% )
       926,367,523      branches:u                #  893.483 M/sec                    ( +-  0.00% )
        21,233,163      branch-misses:u           #    2.29% of all branches          ( +-  0.24% )

       1.459620806 seconds time elapsed                                          ( +-  0.73% )

If you profile it, you'll see that the issue is keeping track of the wordcount.
Using a map/unordered map is going to be faster, but uses more memory. This
is probably /the/ most common tradeoff you'll have to make as a programmer:
Either your code uses more memory and is faster, or it uses less but is slower.
