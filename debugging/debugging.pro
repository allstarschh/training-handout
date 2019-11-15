TEMPLATE = subdirs
SUBDIRS += \
           ex_leak \
           ex_invalid_readwrite \
            \
            \
           ex_datarace \
            \
           ex_uninitialized_read \
           ex_undefined_behavior \
            \
            \
           ex_static_assertions_cpp \
            \
            \
            \
            \
           ex_debugger_cpp \
           ex_breakpoint_commands \
           ex_refcounting \
           ex_gdb_pretty_printer \
           ex_coverage \
           ex_trace \
           ex_watchpoint \
           lab_cache \
           sol_cache \
           lab_dialog \
           sol_dialog \
            \
            \
           ex_random_behavior \
           lab_cache_for_rr \

!macx {
    # Enabling _LIBCPP_DEBUG needs a special libc++ debugging support library which our
    # Mac CI doesn't have, so exclude the example from Mac builds.
    SUBDIRS += ex_assertions_cpp
}

qtHaveModule(quick) {
    SUBDIRS += \
            \
            \
            \
}

greaterThan(QT_MINOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 3) {
    SUBDIRS += 
}

