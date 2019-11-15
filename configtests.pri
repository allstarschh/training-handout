# configure style tests
# QMAKE_CONFIG_TESTS_DIR defaults to dir of current .pro files + config.tests
# sets config_$test if successful, e.g. config_cpp11 for test config.tests/cpp11

load(configure)

# HACK HACK HACK:
# Using qtCompileTest() doesn't pass use the QMake spec passed on the command line.
# For example, when passing "-spec linux-clang" to qmake, the tests will nevertheless
# be run with the spec that Qt was built with.
# As a result, the C++14 tests will fail even when passing "-spec linux-clang" to qmake,
# even when having Clang 3.5 installed.
# The forked myCompileTest() invokes qmake with the correct spec when building the
# config tests.
#
# Upstream bug report at https://bugreports.qt.io/browse/QTBUG-41508,
# fixed in Qt 5.4.
include(mycompiletest.pri)

log("Running configure tests...$$escape_expand(\\n)")
CONFIG_TESTS = $$files($$QMAKE_CONFIG_TESTS_DIR/*.pro, true)
for(test, CONFIG_TESTS) {
    test = $$basename(test)
    test ~= s/\\.pro$//
    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 3) {
        qtCompileTest($$test)
    } else {
        myCompileTest($$test)
    }
}
