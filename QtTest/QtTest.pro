# No C++ sources to build, so use the "subdirs" template without
# any SUBDIRS to disable trying to build a C++ app, lib, or plugin.
TEMPLATE = subdirs

qdeclarativesources.files += \
    qmldir \
    TestCase.qml \
    testlogger.js

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/QtTest

INSTALLS += qdeclarativesources
