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
#include "qdeclarativetestresult_p.h"
#include <QApplication>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/qurl.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qdiriterator.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qeventloop.h>
#include <QtGui/qtextdocument.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

class QTestQuitObject : public QObject
{
    Q_OBJECT
public:
    QTestQuitObject(QObject *parent = 0) : QObject(parent), hasQuit(false) {}

    bool hasQuit;

private Q_SLOTS:
    void quit() { hasQuit = true; }
};

int qtest_quick_main(int argc, char **argv, const char *name, qtest_create_viewport createViewport)
{
    QApplication app(argc, argv);

    // Determine where to look for the test data.  If QTEST_QUICK_SOURCE_DIR
    // is set, then use that.  Otherwise scan the application's resources.
    QString testPath = QString::fromLocal8Bit(qgetenv("QTEST_QUICK_SOURCE_DIR"));
    if (testPath.isEmpty())
        testPath = QLatin1String(":/");

    // Scan the test data directory recursively, looking for "tst_*.qml" files.
    QStringList filters;
    filters += QLatin1String("tst_*.qml");
    QStringList files;
    QDirIterator iter(testPath, filters, QDir::Files,
                      QDirIterator::Subdirectories |
                      QDirIterator::FollowSymlinks);
    while (iter.hasNext())
        files += iter.next();
    files.sort();

    // Bail out if we didn't find any test cases.
    if (files.isEmpty()) {
        qWarning() << argv[1] << ": could not find any test cases under"
                   << testPath;
        return 1;
    }

    // Parse the command-line arguments.
    QDeclarativeTestResult::parseArgs(argc, argv);
    QDeclarativeTestResult::setProgramName(name);

    // Scan through all of the "tst_*.qml" files and run each of them
    // in turn with a QDeclarativeView.
    bool compileFail = false;
    foreach (QString file, files) {
        QFileInfo fi(file);
        if (!fi.exists())
            continue;
        QDeclarativeView view;
        QTestQuitObject quitobj;
        QEventLoop eventLoop;
        QObject::connect(view.engine(), SIGNAL(quit()),
                         &quitobj, SLOT(quit()));
        QObject::connect(view.engine(), SIGNAL(quit()),
                         &eventLoop, SLOT(quit()));
        if (createViewport)
            view.setViewport((*createViewport)());
        QString path = fi.absoluteFilePath();
        if (path.startsWith(QLatin1String(":/")))
            view.setSource(QUrl(QLatin1String("qrc:") + path.mid(2)));
        else
            view.setSource(QUrl::fromLocalFile(path));
        if (view.status() == QDeclarativeView::Error) {
            // Error compiling the test - flag failure and continue.
            compileFail = true;
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

    // Flush the current logging stream.
    QDeclarativeTestResult::setProgramName(0);

    // Return the number of failures as the exit code.
    int code = QDeclarativeTestResult::exitCode();
    if (!code && compileFail)
        ++code;
    return code;
}

QT_END_NAMESPACE

#include "qdeclarativetest.moc"
