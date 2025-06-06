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
#include "settings.h"
#include <QtCore/QEvent>
#include <QtGui/QCursor>
#include <QtGui/QGuiApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QWindow>
#include <QtGui/QPainter>

static const QColor constDarkEdgeColor(64, 64, 64);
static const QColor constLightEdgeColor(192, 192, 192);

static bool isDark = true;

void Edge::setDark(bool d) {
    isDark = d;
}

Edge::Edge(Qt::Edges e, int sz, QWidget *p)
    : QWidget(p)
    , edge(e)
    , size(sz) {
    setAttribute(Qt::WA_Hover);
    update();
}

bool Edge::event(QEvent *ev) {
    switch (ev->type()) {
        case QEvent::Paint:
            if (!parentWidget()->isMaximized()) {
                QPainter p;
                QRect r(rect());
                p.begin(this);
                p.setPen(isDark ? constDarkEdgeColor : constLightEdgeColor);
                switch (edge) {
                    case Qt::LeftEdge:
                        p.drawLine(r.topLeft(), r.bottomLeft());
                        p.drawLine(r.x()+1, r.y(), r.x()+2, r.y());
                        p.drawLine(r.x()+1, r.bottom(), r.x()+2, r.bottom());
                        break;
                    case Qt::TopEdge:
                        p.drawLine(r.topLeft(), r.topRight());
                        break;
                    case Qt::RightEdge:
                        p.drawLine(r.topRight(), r.bottomRight());
                        p.drawLine(r.right()-2, r.y(), r.right()-1, r.y());
                        p.drawLine(r.right()-2, r.bottom(), r.right()-1, r.bottom());
                        break;
                    case Qt::BottomEdge:
                        p.drawLine(r.bottomLeft(), r.bottomRight());
                }
                p.end();
            }
            break;
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
    switch (edge) {
        case Qt::LeftEdge:
            resize(size, ps.height());
            move(0, 0);
            break;
        case Qt::TopEdge:
            resize(ps.width()-(2*(size-1)), size);
            move(size-1, 0);
            break;
        case Qt::RightEdge:
            resize(size, ps.height());
            move(ps.width()-size, 0);
            break;
        case Qt::BottomEdge:
            resize(ps.width()-(2*(size-1)), size);
            move(size-1, ps.height()-size);
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
        QGuiApplication::focusWindow()->startSystemMove();
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
    QGuiApplication::focusWindow()->startSystemResize(edges);
}

void Edge::changeCursorShape(const QPointF &p) {
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
