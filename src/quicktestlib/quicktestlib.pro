TEMPLATE = lib
TARGET = QtTestQuick$${QT_LIBINFIX}
CONFIG += dll warn_on
QT += declarative
DESTDIR = ../../lib

win32 {
    CONFIG += debug_and_release debug_and_release_config build_all
    DLLDESTDIR = ../../bin
    !static:DEFINES += QT_MAKEDLL

    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    DEFINES += QT_MAKEDLL
    CONFIG += epocallowdlldata
    contains(QT_EDITION, OpenSource) {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment WriteUserData
    } else {
        TARGET.CAPABILITY = All -Tcb
    }
}

SOURCES += \
    qdeclarativetest.cpp \
    qdeclarativetestreport.cpp \
    qdeclarativetestresult.cpp
HEADERS += \
    quicktestglobal.h \
    qdeclarativetest.h
PRIVATE_HEADERS += \
    qdeclarativetestreport_p.h \
    qdeclarativetestresult_p.h

PUBLIC_HEADERS += $$HEADERS
HEADERS += $$PRIVATE_HEADERS

include(testlib/testlib.pri)

DEFINES += QT_BUILD_TEST_QUICK_LIB

!symbian {
    target.path += $$[QT_INSTALL_LIBS]
    INSTALLS += target

    install_headers.path = $$[QT_INSTALL_HEADERS]/QtTestQuick
    install_headers.files = $$PUBLIC_HEADERS
    INSTALLS += install_headers
} else {
    load(data_caging_paths)

    quicktestMwHeaders = *.h
    for(api, quicktestMwHeaders) {
        entries=$$files($$api);
        #files() attaches a ';' at the end which needs to be removed
        entries=$$replace(entries, ;,)
        for(entry, entries) {
            exists($$entry) {
                contains(PUBLIC_HEADERS, $$basename(entry)) {
                    BLD_INF_RULES.prj_exports += "$$entry $$MW_LAYER_PUBLIC_EXPORT_PATH(QtTestQuick/$$basename(entry))"
                }
            }
        }
    }
}
