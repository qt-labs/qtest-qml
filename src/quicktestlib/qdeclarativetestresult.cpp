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

#include "qdeclarativetestresult_p.h"
#include "qtestcase.h"
#include "qtestresult_p.h"
#include "qtesttable_p.h"
#include "qtestlog_p.h"
#include <QtCore/qset.h>
#include <QtCore/qmap.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

static const char *globalProgramName = 0;
static bool loggingStarted = false;

class QDeclarativeTestResultPrivate
{
public:
    QDeclarativeTestResultPrivate()
        : table(0)
    {
    }
    ~QDeclarativeTestResultPrivate()
    {
        delete table;
    }

    QByteArray intern(const QString &str);
    void updateTestObjectName();

    QString testCaseName;
    QString functionName;
    QSet<QByteArray> internedStrings;
    QTestTable *table;
};

QByteArray QDeclarativeTestResultPrivate::intern(const QString &str)
{
    QByteArray bstr = str.toUtf8();
    return *(internedStrings.insert(bstr));
}

void QDeclarativeTestResultPrivate::updateTestObjectName()
{
    // In plain logging mode we use the TestCase name as the
    // class name so that multiple TestCase elements will report
    // results with "testCase::function".  In XML logging mode,
    // we use the program name as the class name and report test
    // functions as "testCase__function".
    if (QTestLog::logMode() == QTestLog::Plain) {
        if (testCaseName.isEmpty()) {
            QTestResult::setCurrentTestObject(globalProgramName);
        } else if (QTestLog::logMode() == QTestLog::Plain) {
            QTestResult::setCurrentTestObject
                (intern(testCaseName).constData());
        }
    } else {
        QTestResult::setCurrentTestObject(globalProgramName);
    }
}

QDeclarativeTestResult::QDeclarativeTestResult(QObject *parent)
    : QObject(parent), d_ptr(new QDeclarativeTestResultPrivate)
{
}

QDeclarativeTestResult::~QDeclarativeTestResult()
{
}

/*!
    \qmlproperty string TestResult::testCaseName

    This property defines the name of current TestCase element
    that is running test cases.

    \sa functionName
*/
QString QDeclarativeTestResult::testCaseName() const
{
    Q_D(const QDeclarativeTestResult);
    return d->testCaseName;
}

void QDeclarativeTestResult::setTestCaseName(const QString &name)
{
    Q_D(QDeclarativeTestResult);
    d->testCaseName = name;
    d->updateTestObjectName();
    emit testCaseNameChanged();
}

/*!
    \qmlproperty string TestResult::functionName

    This property defines the name of current test function
    within a TestCase element that is running.  If this string is
    empty, then no function is currently running.

    \sa testCaseName
*/
QString QDeclarativeTestResult::functionName() const
{
    Q_D(const QDeclarativeTestResult);
    return d->functionName;
}

void QDeclarativeTestResult::setFunctionName(const QString &name)
{
    Q_D(QDeclarativeTestResult);
    if (!name.isEmpty()) {
        // In plain logging mode, we use the function name directly.
        // In XML logging mode, we use "testCase__functionName" as the
        // program name is acting as the class name.
        if (QTestLog::logMode() == QTestLog::Plain ||
                d->testCaseName.isEmpty()) {
            QTestResult::setCurrentTestFunction
                (d->intern(name).constData());
        } else {
            QString fullName = d->testCaseName + QLatin1String("__") + name;
            QTestResult::setCurrentTestFunction
                (d->intern(fullName).constData());
        }
    } else {
        QTestResult::setCurrentTestFunction(0);
    }
    emit functionNameChanged();
}

QDeclarativeTestResult::FunctionType QDeclarativeTestResult::functionType() const
{
    return FunctionType(QTestResult::currentTestLocation());
}

void QDeclarativeTestResult::setFunctionType(FunctionType type)
{
    QTestResult::setCurrentTestLocation(QTestResult::TestLocation(type));
    emit functionTypeChanged();
}

/*!
    \qmlproperty string TestResult::dataTag

    This property defines the tag for the current row in a
    data-driven test, or an empty string if not a data-driven test.
*/
QString QDeclarativeTestResult::dataTag() const
{
    const char *tag = QTestResult::currentDataTag();
    if (tag)
        return QString::fromUtf8(tag);
    else
        return QString();
}

void QDeclarativeTestResult::setDataTag(const QString &tag)
{
    if (!tag.isEmpty()) {
        QTestData *data = &(QTest::newRow(tag.toUtf8().constData()));
        QTestResult::setCurrentTestData(data);
        emit dataTagChanged();
    } else {
        QTestResult::setCurrentTestData(0);
    }
}

/*!
    \qmlproperty bool TestResult::failed

    This property returns true if the current test function has
    failed; false otherwise.  The fail state is reset when
    functionName is changed or finishTestFunction() is called.

    \sa skipped, dataFailed
*/
bool QDeclarativeTestResult::isFailed() const
{
    return QTestResult::testFailed();
}

/*!
    \qmlproperty bool TestResult::dataFailed

    This property returns true if the current data function has
    failed; false otherwise.  The fail state is reset when
    functionName is changed or finishTestFunction() is called.

    \sa failed
*/
bool QDeclarativeTestResult::isDataFailed() const
{
    return QTestResult::currentTestFailed();
}

/*!
    \qmlproperty bool TestResult::skipped

    This property returns true if the current test function was
    marked as skipped; false otherwise.

    \sa failed
*/
bool QDeclarativeTestResult::isSkipped() const
{
    return QTestResult::skipCurrentTest();
}

void QDeclarativeTestResult::setSkipped(bool skip)
{
    QTestResult::setSkipCurrentTest(skip);
    emit skippedChanged();
}

/*!
    \qmlproperty int TestResult::passCount

    This property returns the number of tests that have passed.

    \sa failCount, skipCount
*/
int QDeclarativeTestResult::passCount() const
{
    return QTestResult::passCount();
}

/*!
    \qmlproperty int TestResult::failCount

    This property returns the number of tests that have failed.

    \sa passCount, skipCount
*/
int QDeclarativeTestResult::failCount() const
{
    return QTestResult::failCount();
}

/*!
    \qmlproperty int TestResult::skipCount

    This property returns the number of tests that have been skipped.

    \sa passCount, failCount
*/
int QDeclarativeTestResult::skipCount() const
{
    return QTestResult::skipCount();
}

/*!
    \qmlmethod TestResult::reset()

    Resets all pass/fail/skip counters and prepare for testing.
*/
void QDeclarativeTestResult::reset()
{
    if (!globalProgramName)     // Only if run via qmlviewer.
        QTestResult::reset();
}

/*!
    \qmlmethod TestResult::startLogging()

    Starts logging to the test output stream and writes the
    test header.

    \sa stopLogging()
*/
void QDeclarativeTestResult::startLogging()
{
    // The program name is used for logging headers and footers if it
    // is set.  Otherwise the test case name is used.
    Q_D(QDeclarativeTestResult);
    if (loggingStarted)
        return;
    const char *saved = QTestResult::currentTestObjectName();
    if (globalProgramName) {
        QTestResult::setCurrentTestObject(globalProgramName);
    } else {
        QTestResult::setCurrentTestObject
            (d->intern(d->testCaseName).constData());
    }
    QTestLog::startLogging();
    QTestResult::setCurrentTestObject(saved);
    loggingStarted = true;
}

/*!
    \qmlmethod TestResult::stopLogging()

    Writes the test footer to the test output stream and then stops logging.

    \sa startLogging()
*/
void QDeclarativeTestResult::stopLogging()
{
    Q_D(QDeclarativeTestResult);
    if (globalProgramName)
        return;     // Logging will be stopped by setProgramName(0).
    const char *saved = QTestResult::currentTestObjectName();
    QTestResult::setCurrentTestObject(d->intern(d->testCaseName).constData());
    QTestLog::stopLogging();
    QTestResult::setCurrentTestObject(saved);
}

void QDeclarativeTestResult::initTestTable()
{
    Q_D(QDeclarativeTestResult);
    delete d->table;
    d->table = new QTestTable;
}

void QDeclarativeTestResult::clearTestTable()
{
    Q_D(QDeclarativeTestResult);
    delete d->table;
    d->table = 0;
}

void QDeclarativeTestResult::finishTestFunction()
{
    QTestResult::finishedCurrentTestFunction();
}

void QDeclarativeTestResult::fail(const QString &message)
{
    QTestResult::addFailure(message.toLatin1().constData(), "", 0);
}

bool QDeclarativeTestResult::verify(bool success, const QString &message)
{
    if (message.isEmpty()) {
        return QTestResult::verify(success, "verify()", "", "", 0);
    } else {
        return QTestResult::verify
            (success, message.toLatin1().constData(), "", "", 0);
    }
}

bool QDeclarativeTestResult::compare
    (bool success, const QString &message,
     const QString &val1, const QString &val2)
{
    return QTestResult::compare
        (success, message.toLocal8Bit().constData(),
         QTest::toString(val1.toLatin1().constData()),
         QTest::toString(val2.toLatin1().constData()),
         "", "", "", 0);
}

void QDeclarativeTestResult::skipSingle(const QString &message)
{
    QTestResult::addSkip(message.toLatin1().constData(),
                         QTest::SkipSingle, "", 0);
}

void QDeclarativeTestResult::skipAll(const QString &message)
{
    QTestResult::addSkip(message.toLatin1().constData(),
                         QTest::SkipAll, "", 0);
    QTestResult::setSkipCurrentTest(true);
}

bool QDeclarativeTestResult::expectFail
    (const QString &tag, const QString &comment)
{
    return QTestResult::expectFail
        (tag.toLatin1().constData(),
         QTest::toString(comment.toLatin1().constData()),
         QTest::Abort, "", 0);
}

bool QDeclarativeTestResult::expectFailContinue
    (const QString &tag, const QString &comment)
{
    return QTestResult::expectFail
        (tag.toLatin1().constData(),
         QTest::toString(comment.toLatin1().constData()),
         QTest::Continue, "", 0);
}

void QDeclarativeTestResult::warn(const QString &message)
{
    QTestLog::warn(message.toLatin1().constData());
}

void QDeclarativeTestResult::ignoreWarning(const QString &message)
{
    QTestResult::ignoreMessage(QtWarningMsg, message.toLatin1().constData());
}

namespace QTest {
    void qtest_qParseArgs(int argc, char *argv[]);
};

void QDeclarativeTestResult::parseArgs(int argc, char *argv[])
{
    QTest::qtest_qParseArgs(argc, argv);
}

void QDeclarativeTestResult::setProgramName(const char *name)
{
    if (name) {
        QTestResult::reset();
    } else if (!name && loggingStarted) {
        QTestResult::setCurrentTestObject(globalProgramName);
        QTestLog::stopLogging();
        QTestResult::setCurrentTestObject(0);
    }
    globalProgramName = name;
}

int QDeclarativeTestResult::exitCode()
{
#if defined(QTEST_NOEXITCODE)
    return 0;
#else
    // make sure our exit code is never going above 127
    // since that could wrap and indicate 0 test fails
    return qMin(QTestResult::failCount(), 127);
#endif
}

QT_END_NAMESPACE
