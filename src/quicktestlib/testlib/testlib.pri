INCLUDEPATH += $$PWD
VPATH += $$PWD

# Turn off the Q_TESTLIB_EXPORT macro
DEFINES += QTEST_EMBED

# Input
HEADERS += qbenchmark.h \
    qsignalspy.h \
    qtestaccessible.h \
    qtestassert.h \
    qtestbasicstreamer.h \
    qtestcase.h \
    qtestcoreelement.h \
    qtestcorelist.h \
    qtestdata.h \
    qtestelementattribute.h \
    qtestelement.h \
    qtestevent.h \
    qtesteventloop.h \
    qtestfilelogger.h \
    qtest_global.h \
    qtest_gui.h \
    qtest.h \
    qtestkeyboard.h \
    qtestlightxmlstreamer.h \
    qtestmouse.h \
    qtestspontaneevent.h \
    qtestsystem.h \
    qtesttouch.h \
    qtestxmlstreamer.h \
    qtestxunitstreamer.h
SOURCES += qtestcase.cpp \
    qtestlog.cpp \
    qtesttable.cpp \
    qtestdata.cpp \
    qtestresult.cpp \
    qasciikey.cpp \
    qplaintestlogger.cpp \
    qxmltestlogger.cpp \
    qsignaldumper.cpp \
    qabstracttestlogger.cpp \
    qbenchmark.cpp \
    qbenchmarkmeasurement.cpp \
    qbenchmarkvalgrind.cpp \
    qbenchmarkevent.cpp \
    qbenchmarkmetric.cpp \
    qtestelement.cpp \
    qtestelementattribute.cpp \
    qtestbasicstreamer.cpp \
    qtestxunitstreamer.cpp \
    qtestxmlstreamer.cpp \
    qtestlightxmlstreamer.cpp \
    qtestlogger.cpp \
    qtestfilelogger.cpp
DEFINES *= QT_NO_CAST_TO_ASCII \
    QT_NO_CAST_FROM_ASCII \
    QTESTLIB_MAKEDLL \
    QT_NO_DATASTREAM
embedded:QMAKE_CXXFLAGS += -fno-rtti
wince*::LIBS += libcmt.lib \
    corelibc.lib \
    ole32.lib \
    oleaut32.lib \
    uuid.lib \
    commctrl.lib \
    coredll.lib \
    winsock.lib
mac:LIBS += -framework IOKit \
    -framework ApplicationServices \
    -framework Security
