/*
 * Melodeon - LMS Controller
 *
 * Copyright (c) 2022-2023 Craig Drummond <craig.p.drummond@gmail.com>
 *
 * ----
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "edge.h"
#include "config.h"
#include <QtCore/QEvent>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QWindow>

#include <QtCore/QDebug>

Edge::Edge(Qt::Edges e, int sz, QWidget *p)
    : QWidget(p)
    , edge(e)
    , size(sz) {
    //setAttribute(Qt::WA_Hover);
    update();
}

bool Edge::event(QEvent *ev) {
    if (QEvent::Paint!=ev->type() && QEvent::WindowActivate!=ev->type()&& QEvent::WindowDeactivate!=ev->type()) {
        qWarning() << edge << ev->type();
    }

    switch (ev->type()) {
        case QEvent::Enter:
        case QEvent::HoverMove:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            changeCursorShape(static_cast<QHoverEvent *>(ev)->position());
#else
            changeCursorShape(static_cast<QHoverEvent *>(ev)->pos());
#endif
            return true;
        case QEvent::HoverLeave:
            setCursor(Qt::ArrowCursor);
            break;
        case QEvent::MouseButtonPress:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            resizeOrMoveWindow(static_cast<QMouseEvent *>(ev)->position());
#else
            resizeOrMoveWindow(static_cast<QMouseEvent *>(ev)->pos());
#endif
            return true;
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            resizeOrMoveWindow(static_cast<QTouchEvent *>(ev)->points().first().position());
#else
            resizeOrMoveWindow(static_cast<QTouchEvent *>(ev)->touchPoints().first().pos());
#endif
            return true;
        case QEvent::TouchEnd:
            return true;
    }
    return QWidget::event(ev);
}

void Edge::update() {
    QSize ps = qobject_cast<QWidget *>(parent())->size();
qWarning() << ps;
    switch (edge) {
        case Qt::LeftEdge:
            resize(size, ps.height());
            move(0, 0);
            break;
        case Qt::TopEdge:
            resize(ps.width()-(2*size), size);
            move(size, 0);
            break;
        case Qt::RightEdge:
            resize(size, ps.height());
            move(ps.width()-size, 0);
            break;
        case Qt::BottomEdge:
            resize(ps.width()-(2*size), size);
            move(size, ps.height()-size);
    }
}

void Edge::mouseDoubleClickEvent(QMouseEvent *event) {
    if (edge==Qt::TopEdge) {
        QWidget *pw = qobject_cast<QWidget *>(parent());
        if (pw->isMaximized()) {
            pw->showNormal();
        } else {
            pw->showMaximized();
        }
    }
}

void Edge::resizeOrMoveWindow(const QPointF &p) {
    Qt::Edges edges = edge;
    if (edge==Qt::TopEdge && p.y()>2 && p.x()>size && p.x()<width()-size) {
        qobject_cast<QWidget *>(parent())->windowHandle()->startSystemMove();
        return;
    }
    if (edge==Qt::TopEdge || edge==Qt::BottomEdge) {
        if (p.x() >= width() - size) {
            edges |= Qt::RightEdge;
        }
        if (p.x() <= size) {
            edges |= Qt::LeftEdge;
        }
    } else {
        if (p.y() <= size) {
            edges |= Qt::TopEdge;
        }
        if (p.y() >= height() - size) {
            edges |= Qt::BottomEdge;
        }
    }
    qobject_cast<QWidget *>(parent())->windowHandle()->startSystemResize(edges);
}

void Edge::changeCursorShape(const QPointF &p) {
    qWarning() << edge << p;
        Qt::CursorShape shape =
        edge==Qt::TopEdge && p.y()>2 && p.x()>size && p.x()<width()-size
            ? Qt::SizeAllCursor
        : p.x()<0 || p.y()<0 || p.x()>width() || p.y()>height()
            ? Qt::ArrowCursor
        : p.x() < size && p.y() < size || p.x() >= width() - size && p.y() >= height() - size
            ? Qt::SizeFDiagCursor
        : p.x() >= width() - size && p.y() < size || p.x() < size && p.y() >= height() - size
            ? Qt::SizeBDiagCursor
        : p.x() < size || p.x() >= width() - size
            ? Qt::SizeHorCursor
        : p.y() < size || p.y() >= height() - size
            ? Qt::SizeVerCursor
        : Qt::ArrowCursor;

    if (cursor().shape() != shape) {
        setCursor(shape);
    }
}