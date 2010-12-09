TEMPLATE=app
TARGET=tst_qmlauto
CONFIG += warn_on testcase
SOURCES += tst_qmlauto.cpp
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
include(../../src/quicktestlib/quicktestlib_dep.pri)
