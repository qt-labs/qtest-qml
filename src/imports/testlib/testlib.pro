TEMPLATE = lib
TARGET  = qmltestplugin
CONFIG += qt plugin

symbian {
    CONFIG += epocallowdlldata
    contains(QT_EDITION, OpenSource) {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment WriteUserData
    } else {
        TARGET.CAPABILITY = All -Tcb
    }
}

QT += declarative script

SOURCES += main.cpp
HEADERS +=

qdeclarativesources.files += \
    qmldir \
    TestCase.qml \
    SignalSpy.qml \
    testlogger.js

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/QtQuickTest
target.path += $$[QT_INSTALL_IMPORTS]/QtQuickTest
INSTALLS += qdeclarativesources target

include(../../quicktestlib/quicktestlib_dep.pri)
