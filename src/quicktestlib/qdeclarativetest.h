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

#ifndef QDECLARATIVETEST_H
#define QDECLARATIVETEST_H

#include "quicktestglobal.h"
#include <QtGui/qwidget.h>
#ifdef QT_OPENGL_LIB
#include <QtOpenGL/qgl.h>
#endif

QT_BEGIN_NAMESPACE

typedef QWidget *(*qtest_create_viewport)();

Q_TEST_QUICK_EXPORT int qtest_quick_main(int argc, char **argv, const char *name, qtest_create_viewport createViewport, const char *sourceDir);

#ifdef QTEST_QUICK_SOURCE_DIR

#define QTEST_QUICK_MAIN(name) \
    int main(int argc, char **argv) \
    { \
        return qtest_quick_main(argc, argv, #name, 0, QTEST_QUICK_SOURCE_DIR); \
    }

#define QTEST_QUICK_OPENGL_MAIN(name) \
    static QWidget *name##_create_viewport() \
    { \
        return new QGLWidget(); \
    } \
    int main(int argc, char **argv) \
    { \
        return qtest_quick_main(argc, argv, #name, name##_create_viewport, QTEST_QUICK_SOURCE_DIR); \
    }

#else

#define QTEST_QUICK_MAIN(name) \
    int main(int argc, char **argv) \
    { \
        return qtest_quick_main(argc, argv, #name, 0, 0); \
    }

#define QTEST_QUICK_OPENGL_MAIN(name) \
    static QWidget *name##_create_viewport() \
    { \
        return new QGLWidget(); \
    } \
    int main(int argc, char **argv) \
    { \
        return qtest_quick_main(argc, argv, #name, name##_create_viewport, 0); \
    }

#endif

QT_END_NAMESPACE

#endif
