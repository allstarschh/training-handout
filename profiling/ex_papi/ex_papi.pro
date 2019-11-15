TEMPLATE = app

CONFIG += release force_debug_info c++11 console
SOURCES += ex_papi.cpp
DEFINES += QT_MESSAGELOGCONTEXT

!contains(CONFIG, done_config_papi) {
    # support loading this example as stand-alone
    load(configure)
    qtCompileTest("papi")
}

contains(CONFIG, config_papi) {
  message("PAPI support enabled")
  DEFINES += HAVE_PAPI=1
  LIBS += -lpapi
} else {
  warning("PAPI support disabled")
}

