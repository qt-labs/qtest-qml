/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativetest.h"
#include "qdeclarativetestreport.h"
#include <QApplication>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/qurl.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qeventloop.h>
#include <QtGui/qtextdocument.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

// Defined in qdeclarativetestreport.cpp.
extern bool qtest_quick_xmlOutput;
extern int qtest_quick_passed;
extern int qtest_quick_failed;
extern int qtest_quick_skipped;
extern FILE *qtest_quick_stream;

class QTestQuitObject : public QObject
{
    Q_OBJECT
public:
    QTestQuitObject(QObject *parent = 0) : QObject(parent), hasQuit(false) {}

    bool hasQuit;

private Q_SLOTS:
    void quit() { hasQuit = true; }
};

int qtest_quick_main(int argc, char **argv, const char *name, const char *sourceDir, qtest_create_viewport createViewport)
{
    QApplication app(argc, argv);

    // Parse the command-line arguments.
    const char *filename = 0;
    for (int index = 1; index < argc; ++index) {
        QString arg = QString::fromLocal8Bit(argv[index]);
        if (arg == QLatin1String("-xml"))
            qtest_quick_xmlOutput = true;
        else if (arg == QLatin1String("-o") && (index + 1) < argc)
            filename = argv[++index];
    }

    // Determine where to look for the test data.  On a device it will
    // typically be necessary to set QTEST_QUICK_SOURCE_DIR.
    QString testPath = QString::fromLocal8Bit(qgetenv("QTEST_QUICK_SOURCE_DIR"));
    if (testPath.isEmpty() && sourceDir)
        testPath = QString::fromLocal8Bit(sourceDir);
    if (!testPath.isEmpty() && !QFile::exists(testPath))
        testPath = QString();
    if (testPath.isEmpty())
        testPath = QLatin1String(".");

    // Find the subdirectories that look like they may contain test cases.
    // We also include "." in this list.
    QDir dir(testPath);
    QStringList entries = dir.entryList(QDir::Dirs);
    entries.removeAll(QLatin1String(".."));
    if (!entries.contains(QLatin1String(".")))
        entries.append(QLatin1String("."));

    if (filename) {
        qtest_quick_stream = fopen(filename, "w");
        if (!qtest_quick_stream) {
            perror(filename);
            return 1;
        }
    } else {
        qtest_quick_stream = stdout;
    }

    if (qtest_quick_xmlOutput) {
        fprintf(qtest_quick_stream, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                        "<TestCase name=\"%s\">\n", name);
        fprintf(qtest_quick_stream, "<Environment>\n"
                        "   <QtVersion>%s</QtVersion>\n"
                        "   <QTestVersion>%s</QTestVersion>\n"
                        "</Environment>\n", qVersion(), qVersion());
    } else {
        fprintf(qtest_quick_stream, "********* Start testing of %s *********\n", name);
    }

    // Scan through all of the "tst_*.qml" files in the subdirectories
    // and run each of them in turn with a QDeclarativeView.
    QStringList filters;
    filters += QLatin1String("tst_*.qml");
    foreach (QString name, entries) {
        QDir subdir(testPath + QDir::separator() + name);
        QStringList files = subdir.entryList(filters, QDir::Files);
        foreach (QString file, files) {
            QString source = subdir.path() + QDir::separator() + file;
            QFileInfo fi(source);
            if (fi.exists()) {
                QDeclarativeView view;
                QTestQuitObject quitobj;
                QEventLoop eventLoop;
                QObject::connect(view.engine(), SIGNAL(quit()),
                                 &quitobj, SLOT(quit()));
                QObject::connect(view.engine(), SIGNAL(quit()),
                                 &eventLoop, SLOT(quit()));
                if (createViewport)
                    view.setViewport((*createViewport)());
                view.engine()->addImportPath(testPath);
                view.setSource(QUrl::fromLocalFile(fi.absoluteFilePath()));
                if (view.status() == QDeclarativeView::Error) {
                    // Error compiling the test - flag failure and continue.
                    ++qtest_quick_failed;
                    continue;
                }
                if (!quitobj.hasQuit) {
                    // If the test already quit, then it was performed
                    // synchronously during setSource().  Otherwise it is
                    // an asynchronous test and we need to show the window
                    // and wait for the quit indication.
                    view.show();
                    eventLoop.exec();
                }
            }
        }
    }

    if (qtest_quick_xmlOutput) {
        fprintf(qtest_quick_stream, "</TestCase>\n");
    } else {
        fprintf(qtest_quick_stream, "Totals: %d passed, %d failed, %d skipped\n",
                qtest_quick_passed, qtest_quick_failed, qtest_quick_skipped);
        fprintf(qtest_quick_stream, "********* Finished testing of %s *********\n", name);
    }

    if (filename)
        fclose(qtest_quick_stream);

    return qtest_quick_failed != 0;
}

QT_END_NAMESPACE

#include "qdeclarativetest.moc"
