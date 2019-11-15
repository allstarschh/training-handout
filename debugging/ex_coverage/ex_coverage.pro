TEMPLATE = app

CONFIG -= qt
CONFIG += force_debug_info console

gcc|clang {
    CONFIG += gcov
}

SOURCES  += ex_coverage.cpp

