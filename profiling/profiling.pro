TEMPLATE = subdirs
SUBDIRS += \
           ex_string_comparison \
           ex_random \
           ex_sleep \
            \
            \
           ex_timing_cpp \
            \
           ex_container \
           ex_branches \
           ex_listdir \
           ex_papi \
           lab_memory \
           sol_memory \
           lab_latency \
           lab_mandelbrot \
           sol_mandelbrot \
            \
            \

!win32-msvc2010:!win32-msvc2012:!win32-msvc2013 {
    SUBDIRS += \
            ex_sharing
}

qtHaveModule(quick) {
    SUBDIRS += \
            \
            \
            \
            \
}

greaterThan(QT_MINOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 3) {
    SUBDIRS += \
           lab_mandelbrot_concurrent \
           sol_mandelbrot_concurrent \
}

