TEMPLATE = subdirs
SUBDIRS = src tests
CONFIG += ordered

# Install qmltestcase.prf into the Qt mkspecs so that "CONFIG += qmltestcase"
# can be used in customer applications to build against QtQuickTest.
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$PWD/features/qmltestcase.prf
INSTALLS += feature

#symbian does not generate make install rule. Copy prf manually
symbian {
    qmltestcaseFile=$$PWD\\features\\qmltestcase.prf
    qmltestcaseFile=$$replace(qmltestcaseFile,/,\\)
    featuresDir=$$[QT_INSTALL_DATA]\\mkspecs\\features
    featuresDir=$$replace(featuresDir,/,\\)
    system(copy "$$qmltestcaseFile $$featuresDir")
}
