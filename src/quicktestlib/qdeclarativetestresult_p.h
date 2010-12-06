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

#ifndef QDECLARATIVETESTRESULT_P_H
#define QDECLARATIVETESTRESULT_P_H

#include "quicktestglobal.h"
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_NAMESPACE

class QDeclarativeTestResultPrivate;

class Q_TEST_QUICK_EXPORT QDeclarativeTestResult : public QObject
{
    Q_OBJECT
    Q_ENUMS(FunctionType)
    Q_PROPERTY(QString testCaseName READ testCaseName WRITE setTestCaseName NOTIFY testCaseNameChanged)
    Q_PROPERTY(QString functionName READ functionName WRITE setFunctionName NOTIFY functionNameChanged)
    Q_PROPERTY(FunctionType functionType READ functionType WRITE setFunctionType NOTIFY functionTypeChanged)
    Q_PROPERTY(QString dataTag READ dataTag WRITE setDataTag NOTIFY dataTagChanged)
    Q_PROPERTY(bool failed READ isFailed)
    Q_PROPERTY(bool dataFailed READ isDataFailed)
    Q_PROPERTY(bool skipped READ isSkipped WRITE setSkipped NOTIFY skippedChanged)
    Q_PROPERTY(int passCount READ passCount)
    Q_PROPERTY(int failCount READ failCount)
    Q_PROPERTY(int skipCount READ skipCount)
public:
    QDeclarativeTestResult(QObject *parent = 0);
    ~QDeclarativeTestResult();

    // Values must match QTestResult::TestLocation.
    enum FunctionType
    {
        NoWhere = 0,
        DataFunc = 1,
        InitFunc = 2,
        Func = 3,
        CleanupFunc = 4
    };

    QString testCaseName() const;
    void setTestCaseName(const QString &name);

    QString functionName() const;
    void setFunctionName(const QString &name);

    FunctionType functionType() const;
    void setFunctionType(FunctionType type);

    QString dataTag() const;
    void setDataTag(const QString &tag);

    bool isFailed() const;
    bool isDataFailed() const;

    bool isSkipped() const;
    void setSkipped(bool skip);

    int passCount() const;
    int failCount() const;
    int skipCount() const;

public Q_SLOTS:
    void reset();

    void startLogging();
    void stopLogging();

    void initTestTable();
    void clearTestTable();

    void finishTestFunction();

    void fail(const QString &message);
    bool verify(bool success, const QString &message);
    bool compare(bool success, const QString &message,
                 const QString &val1, const QString &val2);
    void skipSingle(const QString &message);
    void skipAll(const QString &message);
    bool expectFail(const QString &tag, const QString &comment);
    bool expectFailContinue(const QString &tag, const QString &comment);
    void warn(const QString &message);

    void ignoreWarning(const QString &message);

public:
    // Helper functions for the C++ main() shell.
    static void parseArgs(int argc, char *argv[]);
    static void setProgramName(const char *name);
    static int exitCode();

Q_SIGNALS:
    void programNameChanged();
    void testCaseNameChanged();
    void functionNameChanged();
    void functionTypeChanged();
    void dataTagChanged();
    void skippedChanged();

private:
    QScopedPointer<QDeclarativeTestResultPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QDeclarativeTestResult)
    Q_DISABLE_COPY(QDeclarativeTestResult)
};

QT_END_NAMESPACE

#endif
