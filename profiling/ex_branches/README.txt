The `ex_branches` example shows the performance impact of branch misses.

By default, the code builds up a pathological case for the branch
predictor of the CPU: The inner loop traverses an array of random
contents where half of the elements fulfil the branch condition.

When you profile the example in this setup, you will be able to
measure a high level of branch misses. E.g. with perf stat:

 Performance counter stats for './ex_branches' (5 runs):

       7379.838168      task-clock:u (msec)       #    1.000 CPUs utilized            ( +-  0.29% )
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               537      page-faults:u             #    0.073 K/sec                    ( +-  0.20% )
    23,473,656,761      cycles:u                  #    3.181 GHz                      ( +-  0.22% )
    14,763,718,955      instructions:u            #    0.63  insn per cycle           ( +-  0.00% )
     3,278,929,853      branches:u                #  444.309 M/sec                    ( +-  0.00% )
       800,629,193      branch-misses:u           #   24.42% of all branches          ( +-  0.02% )

       7.381717163 seconds time elapsed                                          ( +-  0.28% )

Note that the data here measures all branches in the code. The inner loop
actually makes up for ~1,600,00,00 branches, thus we have 50% branch misses
in there. The CPU's predictor cannot do anything against such odds!

When we sort the data before looping over it, the situation improves
dramatically and the CPU's branch predictor shines again:

 Performance counter stats for './ex_branches 1' (5 runs):

       1213.102189      task-clock:u (msec)       #    1.000 CPUs utilized            ( +-  0.19% )
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               538      page-faults:u             #    0.443 K/sec                    ( +-  0.22% )
     3,853,626,937      cycles:u                  #    3.177 GHz                      ( +-  0.15% )
    14,767,242,465      instructions:u            #    3.83  insn per cycle           ( +-  0.00% )
     3,279,804,302      branches:u                # 2703.650 M/sec                    ( +-  0.00% )
           324,241      branch-misses:u           #    0.01% of all branches          ( +-  0.08% )

       1.213531743 seconds time elapsed                                          ( +-  0.19% )

If you experiment with Q_LIKELY/Q_UNLIKELY at this point, you will not see
any effect as the data is distributed such that we have 50/50 odds of taking
the branch. Changing the conditional threshold from 128 to 192 though, i.e.
moving the odds to 25/75, one can observe the positive and negative effects
when giving the CPU the correct or wrong hints.

Just setting the threshold to 192:

 Performance counter stats for './ex_branches':

       4587.524274      task-clock:u (msec)       #    1.000 CPUs utilized
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               536      page-faults:u             #    0.117 K/sec
    14,564,274,857      cycles:u                  #    3.175 GHz
    13,934,317,993      instructions:u            #    0.96  insn per cycle
     3,278,926,959      branches:u                #  714.749 M/sec
       414,454,422      branch-misses:u           #   12.64% of all branches

       4.588030001 seconds time elapsed

Adding Q_LIKELY slightly decreases the performance:

 Performance counter stats for './ex_branches':

       4600.892251      task-clock:u (msec)       #    0.999 CPUs utilized
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               537      page-faults:u             #    0.117 K/sec
    14,610,690,618      cycles:u                  #    3.176 GHz
    13,934,317,980      instructions:u            #    0.95  insn per cycle
     3,278,926,951      branches:u                #  712.672 M/sec
       414,791,288      branch-misses:u           #   12.65% of all branches

       4.603742918 seconds time elapsed

Adding Q_UNLIKELY slightly improves the performance:

 Performance counter stats for './ex_branches':

       4018.449220      task-clock:u (msec)       #    1.000 CPUs utilized
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
               537      page-faults:u             #    0.134 K/sec
    12,745,974,190      cycles:u                  #    3.172 GHz
    13,934,330,582      instructions:u            #    1.09  insn per cycle
     3,278,977,405      branches:u                #  815.981 M/sec
       391,063,762      branch-misses:u           #   11.93% of all branches

       4.019973169 seconds time elapsed
