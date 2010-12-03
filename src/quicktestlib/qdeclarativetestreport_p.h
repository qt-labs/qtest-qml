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

#ifndef QDECLARATIVETESTREPORT_P_H
#define QDECLARATIVETESTREPORT_P_H

#include "quicktestglobal.h"
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtDeclarative/qdeclarative.h>

QT_BEGIN_NAMESPACE

class Q_TEST_QUICK_EXPORT QDeclarativeTestReport : public QObject
{
    Q_OBJECT
public:
    QDeclarativeTestReport(QObject *parent = 0) : QObject(parent) {}

public Q_SLOTS:
    void report(int pass, int fail, int skip);
    void log_fail(const QString &testCase, const QString &message);
    void log_expect_fail
        (const QString &testCase, const QString &message);
    void log_expect_fail_pass(const QString &testCase);
    void log_skip(const QString &testCase, const QString &message);
    void log_pass(const QString &testCase);
    void log_message(const QString &message);

private:
    void log_incident(const char *type, const QString &testCase,
                      const QString &message);
};

QML_DECLARE_TYPE(QDeclarativeTestReport)

QT_END_NAMESPACE

#endif
