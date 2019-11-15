TEMPLATE = app

QT -= gui
# In this case we disable optimizations to avoid any inlining (for better visualisation in heaptrack).
# The use of malloc and free is the same with and without optimizations.
CONFIG += debug force_debug_info console
SOURCES  += ex_string_comparison.cpp

