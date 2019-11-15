TEMPLATE = app

CONFIG += release force_debug_info c++11 console
SOURCES += ex_listdir.cpp
DEFINES += QT_MESSAGELOGCONTEXT

# NOTE: uncomment this to try out the lttng-ust integration
#       Note that this can come with a serious performance overhead
#
# !contains(CONFIG, done_config_lttng) {
#     # support loading this example as stand-alone
#     load(configure)
#     qtCompileTest("lttng")
# }
#
# contains(CONFIG, config_lttng) {
#   message("LTTng support enabled")
#   DEFINES += HAVE_LTTNG=1
#   LIBS += -llttng-ust -ldl
# } else {
#   warning("LTTng support disabled")
# }

