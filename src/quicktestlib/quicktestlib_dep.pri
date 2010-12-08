
QT += declarative

INCLUDEPATH += $$PWD
LIBS += -L../../lib -L../../bin

win32:CONFIG(debug, debug|release) {
    LIBS += -lQtQuickTest$${QT_LIBINFIX}d
} else {
    LIBS += -lQtQuickTest$${QT_LIBINFIX}
}

# Locate the "lib" directory in the build tree and put it before
# the Qt "lib" directory in the library path so that we link
# against the libQtQuickTest.so in our build tree, not the Qt one.
FIND_TOP=..
for(i,forever) {
    exists($$_PRO_FILE_PWD_/$$FIND_TOP/qtest-qml.pro):break()
    FIND_TOP=../$$FIND_TOP
}
load(qt)
QMAKE_LIBDIR=$$OUT_PWD/$$FIND_TOP/lib $$QMAKE_LIBDIR
