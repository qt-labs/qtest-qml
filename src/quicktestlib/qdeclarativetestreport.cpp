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

#include "qdeclarativetestreport.h"
#include <QtGui/qtextdocument.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

static bool xmlOutput = false;
static int passed = 0;
static int failed = 0;
static int skipped = 0;
static FILE *stream = 0;

void QDeclarativeTestReport::report(int pass, int fail, int skip)
{
    passed += pass;
    failed += fail;
    skipped += skip;
}

void QDeclarativeTestReport::log_fail(const QString &testCase, const QString &message)
{
    if (!stream)
        stream = stdout;
    if (xmlOutput) {
        log_incident("fail", testCase, message);
    } else if (!message.isEmpty()) {
        fprintf(stream, "FAIL!  : %s %s\n",
                testCase.toLatin1().constData(),
                message.toLatin1().constData());
    } else {
        fprintf(stream, "FAIL!  : %s\n", testCase.toLatin1().constData());
    }
}

void QDeclarativeTestReport::log_expect_fail
    (const QString &testCase, const QString &message)
{
    if (!stream)
        stream = stdout;
    if (xmlOutput) {
        log_incident("xfail", testCase, message);
    } else if (!message.isEmpty()) {
        fprintf(stream, "XFAIL  : %s %s\n",
                testCase.toLatin1().constData(),
                message.toLatin1().constData());
    } else {
        fprintf(stream, "XFAIL  : %s\n", testCase.toLatin1().constData());
    }
}

void QDeclarativeTestReport::log_expect_fail_pass(const QString &testCase)
{
    if (!stream)
        stream = stdout;
    if (xmlOutput)
        log_incident("xpass", testCase, QString());
    else
        fprintf(stream, "XPASS  : %s\n", testCase.toLatin1().constData());
}

void QDeclarativeTestReport::log_skip(const QString &testCase, const QString &message)
{
    if (!stream)
        stream = stdout;
    if (xmlOutput) {
        log_incident("skip", testCase, message);
    } else if (!message.isEmpty()) {
        fprintf(stream, "SKIP   : %s %s\n",
                testCase.toLatin1().constData(),
                message.toLatin1().constData());
    } else {
        fprintf(stream, "SKIP   : %s\n", testCase.toLatin1().constData());
    }
}

void QDeclarativeTestReport::log_pass(const QString &testCase)
{
    if (!stream)
        stream = stdout;
    if (xmlOutput)
        log_incident("pass", testCase, QString());
    else
        fprintf(stream, "PASS   : %s\n", testCase.toLatin1().constData());
}

void QDeclarativeTestReport::log_message(const QString &message)
{
    if (!stream)
        stream = stdout;
    if (!xmlOutput)
        fprintf(stream, "%s\n", message.toLatin1().constData());
}

void QDeclarativeTestReport::log_incident
    (const char *type, const QString &testCase, const QString &message)
{
    QString name(testCase);
    QString tag;
    name.replace(QLatin1String("()"), QLatin1String(""));
    name.replace(QLatin1String("::"), QLatin1String("__"));
    int tagIndex = name.indexOf(QLatin1String(" ["));
    if (tagIndex >= 0) {
        tag = name.mid(tagIndex + 2);
        if (tag.endsWith(QLatin1String("]")))
            tag = tag.left(tag.length() - 1);
        name = name.left(tagIndex);
    }
    fprintf(stream, "<TestFunction name=\"%s\">\n",
            Qt::escape(name).toLatin1().constData());
    if (message.isEmpty() && tag.isEmpty()) {
        fprintf(stream, "<Incident type=\"%s\" file=\"\" line=\"0\" />\n", type);
    } else {
        fprintf(stream, "<Incident type=\"%s\" file=\"\" line=\"0\">\n", type);
        if (!tag.isEmpty()) {
            fprintf(stream, "    <DataTag>%s</DataTag>\n",
                    Qt::escape(tag).toLatin1().constData());
        }
        if (!message.isEmpty()) {
            fprintf(stream, "    <Description>%s</Description>\n",
                    Qt::escape(message).toLatin1().constData());
        }
        fprintf(stream, "</Incident>\n");
    }
    fprintf(stream, "</TestFunction>\n");
}

QT_END_NAMESPACE
