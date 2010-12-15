TEMPLATE=app
TARGET=tst_qmlexample
CONFIG += warn_on testcase
SOURCES += tst_qmlexample.cpp
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
include(../../src/quicktestlib/quicktestlib_dep.pri)
