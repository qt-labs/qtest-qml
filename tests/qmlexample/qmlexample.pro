load(qttest_p4.prf)
TEMPLATE=app
CONFIG += warn_on
SOURCES += tst_qmlexample.cpp
DEFINES += QTEST_QUICK_SOURCE_DIR=\"\\\"$$PWD\\\"\"
include(../../src/quicktestlib/quicktestlib_dep.pri)
