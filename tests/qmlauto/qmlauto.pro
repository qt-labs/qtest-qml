TEMPLATE=app
TARGET=tst_qmlauto
CONFIG += warn_on testcase
SOURCES += tst_qmlauto.cpp
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
include(../../src/quicktestlib/quicktestlib_dep.pri)

OTHER_FILES += \
    selftests/tst_selftests.qml \
    qdecarativebinding/tst_binding2.qml \
    qdecarativebinding/tst_binding.qml \
    selftests/tst_compare.qml \
    selftests/tst_compare_quickobjects.qml
