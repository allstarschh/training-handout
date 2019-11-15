#! /bin/sh

perf script | stackcollapse-perf.pl | flamegraph.pl > output.svg
