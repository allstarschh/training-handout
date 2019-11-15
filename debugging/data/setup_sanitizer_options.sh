#!/bin/bash
# to use this, just source this script, e.g.:
# $ . path/to/setup_sanitizer_options.sh
# optionaly set fast_unwind_on_malloc=false to get full backtraces even when framepointers are missing
export LSAN_OPTIONS=print_suppressions=0,suppressions=$(readlink -f $(dirname ${BASH_SOURCE[0]})/lsan.supp),fast_unwind_on_malloc=true
export UBSAN_OPTIONS=print_stacktrace=1
