TEMPLATE=app
TARGET=tst_qmlexample
CONFIG += warn_on testcase
SOURCES += tst_qmlexample.cpp
DEFINES += QTEST_QUICK_SOURCE_DIR=\"\\\"$$PWD\\\"\"
include(../../src/quicktestlib/quicktestlib_dep.pri)
