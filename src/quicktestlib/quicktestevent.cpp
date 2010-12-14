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

#include "quicktestevent_p.h"
#include "qtestkeyboard.h"
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicsview.h>

QT_BEGIN_NAMESPACE

QuickTestEvent::QuickTestEvent(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setVisible(false);
}

QuickTestEvent::~QuickTestEvent()
{
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

    static void mouseEvent(MouseAction action, QGraphicsView *widget,
                           QDeclarativeItem *item, Qt::MouseButton button,
                           Qt::KeyboardModifiers stateKey, QPointF _pos, int delay=-1)
    {
        QTEST_ASSERT(widget);
        QTEST_ASSERT(item);

        if (delay == -1 || delay < QTest::defaultMouseDelay())
            delay = QTest::defaultMouseDelay();
        if(delay > 0)
            QTest::qWait(delay);

        if (action == MouseClick) {
            mouseEvent(MousePress, widget, item, button, stateKey, _pos);
            mouseEvent(MouseRelease, widget, item, button, stateKey, _pos);
            return;
        }

        QPoint pos = widget->mapFromScene(item->mapToScene(_pos));

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
                QCursor::setPos(widget->mapToGlobal(pos));
                qApp->processEvents();
                return;
            default:
                QTEST_ASSERT(false);
        }
        QSpontaneKeyEvent::setSpontaneous(&me);
        if (!qApp->notify(widget->viewport(), &me)) {
            static const char *mouseActionNames[] =
                { "MousePress", "MouseRelease", "MouseClick", "MouseDoubleClick", "MouseMove" };
            QString warning = QString::fromLatin1("Mouse event \"%1\" not accepted by receiving widget");
            QTest::qWarn(warning.arg(QString::fromLatin1(mouseActionNames[static_cast<int>(action)])).toAscii().data());
        }
    }
};

bool QuickTestEvent::mousePress
    (QDeclarativeItem *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QGraphicsView *view = qobject_cast<QGraphicsView *>(eventWidget());
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MousePress, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseRelease
    (QDeclarativeItem *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QGraphicsView *view = qobject_cast<QGraphicsView *>(eventWidget());
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseRelease, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseClick
    (QDeclarativeItem *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QGraphicsView *view = qobject_cast<QGraphicsView *>(eventWidget());
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseClick, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseDoubleClick
    (QDeclarativeItem *item, qreal x, qreal y, int button,
     int modifiers, int delay)
{
    QGraphicsView *view = qobject_cast<QGraphicsView *>(eventWidget());
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseDoubleClick, view, item,
                            Qt::MouseButton(button),
                            Qt::KeyboardModifiers(modifiers),
                            QPointF(x, y), delay);
    return true;
}

bool QuickTestEvent::mouseMove
    (QDeclarativeItem *item, qreal x, qreal y, int delay)
{
    QGraphicsView *view = qobject_cast<QGraphicsView *>(eventWidget());
    if (!view)
        return false;
    QtQuickTest::mouseEvent(QtQuickTest::MouseMove, view, item,
                            Qt::NoButton, Qt::NoModifier,
                            QPointF(x, y), delay);
    return true;
}

QWidget *QuickTestEvent::eventWidget()
{
    QGraphicsScene *s = scene();
    if (!s)
        return 0;
    QList<QGraphicsView *> views = s->views();
    if (views.isEmpty())
        return 0;
    return views.at(0);
}

QT_END_NAMESPACE
