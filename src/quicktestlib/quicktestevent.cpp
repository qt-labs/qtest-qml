/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quicktestevent_p.h"
#include "qtestkeyboard.h"
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeitem.h>
#include <QtDeclarative/qdeclarativeview.h>
#if defined(QML_VERSION) && QML_VERSION >= 0x020000
#include <QtDeclarative/qsgitem.h>
#include <QtDeclarative/qsgcanvas.h>
#define QUICK_TEST_SCENEGRAPH 1
#endif
#include <QtGui/qgraphicsscene.h>

QT_BEGIN_NAMESPACE

QuickTestEvent::QuickTestEvent(QObject *parent)
    : QObject(parent)
{
}

QuickTestEvent::~QuickTestEvent()
{
}

int QuickTestEvent::startDragDistance()
{
    return qApp->startDragDistance();
}

bool QuickTestEvent::keyPress(int key, int modifiers, int delay)
{
    QWidget *widget = eventWidget();
    if (!widget)
        return false;
    QTest::keyPress(widget, Qt::Key(key), Qt::KeyboardModifiers(modifiers), delay);
    return true;
}

bool QuickTestEvent::keyRelease(int key, int modifiers, int delay)
{
    QWidget *widget = eventWidget();
    if (!widget)
        return false;
    QTest::keyRelease(widget, Qt::Key(key), Qt::KeyboardModifiers(modifiers), delay);
    return true;
}

bool QuickTestEvent::keyClick(int key, int modifiers, int delay)
{
    QWidget *widget = eventWidget();
    if (!widget)
        return false;
    QTest::keyClick(widget, Qt::Key(key), Qt::KeyboardModifiers(modifiers), delay);
    return true;
}

namespace QTest {
    extern int Q_TESTLIB_EXPORT defaultMouseDelay();
};

namespace QtQuickTest
{
    enum MouseAction { MousePress, MouseRelease, MouseClick, MouseDoubleClick, MouseMove };

    static void mouseEvent(MouseAction action, QWidget *widget,
                           QObject *item, Qt::MouseButton button,
                           Qt::KeyboardModifiers stateKey, QPointF _pos, int delay=-1)
    {
        QTEST_ASSERT(widget);
        QTEST_ASSERT(item);

        if (delay == -1 || delay < QTest::defaultMouseDelay())
            delay = QTest::defaultMouseDelay();
        if (delay > 0)
            QTest::qWait(delay);

        if (action == MouseClick) {
            mouseEvent(MousePress, widget, item, button, stateKey, _pos);
            mouseEvent(MouseRelease, widget, item, button, stateKey, _pos);
            return;
        }

        QPoint pos;
        QDeclarativeView *view = qobject_cast<QDeclarativeView *>(widget);
        QWidget *eventWidget = widget;
#ifdef QUICK_TEST_SCENEGRAPH
        QSGItem *sgitem = qobject_cast<QSGItem *>(item);
        if (sgitem) {
            pos = sgitem->mapToScene(_pos).toPoint();
        } else
#endif
        {
            QDeclarativeItem *ditem = qobject_cast<QDeclarativeItem *>(item);
            if (!ditem) {
                qWarning("Mouse event target is not an Item");
                return;
            }
            pos = view->mapFromScene(ditem->mapToScene(_pos));
            eventWidget = view->viewport();
        }

        QTEST_ASSERT(button == Qt::NoButton || button & Qt::MouseButtonMask);
        QTEST_ASSERT(stateKey == 0 || stateKey & Qt::KeyboardModifierMask);

        stateKey &= static_cast<unsigned int>(Qt::KeyboardModifierMask);

        QMouseEvent me(QEvent::User, QPoint(), Qt::LeftButton, button, stateKey);
        switch (action)
        {
            case MousePress:
                me = QMouseEvent(QEvent::MouseButtonPress, pos, widget->mapToGlobal(pos), button, button, stateKey);
                break;
            case MouseRelease:
                me = QMouseEvent(QEvent::MouseButtonRelease, pos, widget->mapToGlobal(pos), button, 0, stateKey);
                break;
            case MouseDoubleClick:
                me = QMouseEvent(QEvent::MouseButtonDblClick, pos, widget->mapToGlobal(pos), button, button, stateKey);
                break;
            case MouseMove:
                if (button == Qt::NoButton) {
                    // If we send a mouse move event with no button pressed, it will be
                    // rejected (unless mouseTracking is set to true); simulate instead
                    QCursor::setPos(widget->mapToGlobal(pos));
                    qApp->processEvents();
                } else {
                    me = QMouseEvent(QEvent::MouseMove, pos, widget->mapToGlobal(pos), button, button, stateKey);
                }
                break;
            default:
                QTEST_ASSERT(false);
        }
        QSpontaneKeyEvent::setSpontaneous(&me);
        if (!qApp->notify(eventWidget, &me)) {
            static const char *mouseActionNames[] =
                { "MousePress", "MouseRelease", "MouseClick", "MouseDoubleClick", "MouseMove" };
            QString warning = QString::fromLatin1("Mouse event \"%1\" not accepted by receiving widget");
            QTest::qWarn(warning.arg(QString::fromLatin1(mouseActionNames[static_cast<int>(action)])).toAscii().data());
        }
    }
};

bool QuickTestEvent::mousePress
    (QObject *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QWidget *view = eventWidget();
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MousePress, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseRelease
    (QObject *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QWidget *view = eventWidget();
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseRelease, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseClick
    (QObject *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QWidget *view = eventWidget();
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseClick, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseDoubleClick
    (QObject *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QWidget *view = eventWidget();
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseDoubleClick, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseMove
    (QObject *item, qreal x, qreal y, int delay, int buttons)
{
    QWidget *view = eventWidget();
    if (!view)
        return false;

    QtQuickTest::mouseEvent(QtQuickTest::MouseMove, view, item,
                            Qt::MouseButton(buttons), Qt::NoModifier,
                            QPointF(x, y), delay);
    return true;
}

QWidget *QuickTestEvent::eventWidget()
{
#ifdef QUICK_TEST_SCENEGRAPH
    QSGItem *sgitem = qobject_cast<QSGItem *>(parent());
    if (sgitem)
        return sgitem->canvas();
#endif
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(parent());
    if (!item)
        return 0;
    QGraphicsScene *s = item->scene();
    while (!s && item) {
        item = qobject_cast<QDeclarativeItem *>(item->parent());
        s = item->scene();
    }
    if (!s)
        return 0;
    QList<QGraphicsView *> views = s->views();
    if (views.isEmpty())
        return 0;
    return views.at(0);
}

QT_END_NAMESPACE
