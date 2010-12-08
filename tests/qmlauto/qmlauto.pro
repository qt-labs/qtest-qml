TEMPLATE=app
TARGET=tst_qmlauto
CONFIG += warn_on testcase
SOURCES += tst_qmlauto.cpp
DEFINES += QTEST_QUICK_SOURCE_DIR=\"\\\"$$PWD\\\"\"
include(../../src/quicktestlib/quicktestlib_dep.pri)
