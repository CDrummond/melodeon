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

static int constSize = 4;

Edge::Edge(Qt::Edges e, QWidget *p)
    : QWidget(p)
    , edge(e) {
    //setAttribute(Qt::WA_Hover);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(255, 0, 0));
    setPalette(pal);
    update();
}

bool Edge::event(QEvent *ev) {
    qWarning() << edge << ev->type();

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
            resizeWindow(static_cast<QMouseEvent *>(ev)->position());
#else
            resizeWindow(static_cast<QMouseEvent *>(ev)->pos());
#endif
            return true;
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            resizeWindow(static_cast<QTouchEvent *>(ev)->points().first().position());
#else
            resizeWindow(static_cast<QTouchEvent *>(ev)->touchPoints().first().pos());
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
            resize(constSize, ps.height());
            move(0, 0);
            break;
        case Qt::TopEdge:
            resize(ps.width()-(2*constSize), constSize);
            move(constSize, 0);
            break;
        case Qt::RightEdge:
            resize(constSize, ps.height());
            move(ps.width()-constSize, 0);
            break;
        case Qt::BottomEdge:
            resize(ps.width()-(2*constSize), constSize);
            move(constSize, ps.height()-constSize);
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

void Edge::resizeWindow(const QPointF &p) {
    Qt::Edges edges = edge;
    if (edge==Qt::TopEdge || edge==Qt::BottomEdge) {
        if (p.x() >= width() - constSize) {
            edges |= Qt::RightEdge;
        }
        if (p.x() <= constSize) {
            edges |= Qt::LeftEdge;
        }
    } else {
        if (p.y() <= constSize) {
            edges |= Qt::TopEdge;
        }
        if (p.y() >= height() - constSize) {
            edges |= Qt::BottomEdge;
        }
    }
    qobject_cast<QWidget *>(parent())->windowHandle()->startSystemResize(edges);
}

void Edge::changeCursorShape(const QPointF &p) {
    qWarning() << edge << p;
        Qt::CursorShape shape =
        p.x()<0 || p.y()<0 || p.x()>width() || p.y()>height()
            ? Qt::ArrowCursor
        : p.x() < constSize && p.y() < constSize || p.x() >= width() - constSize && p.y() >= height() - constSize
            ? Qt::SizeFDiagCursor
        : p.x() >= width() - constSize && p.y() < constSize || p.x() < constSize && p.y() >= height() - constSize
            ? Qt::SizeBDiagCursor
        : p.x() < constSize || p.x() >= width() - constSize
            ? Qt::SizeHorCursor
        : p.y() < constSize || p.y() >= height() - constSize
            ? Qt::SizeVerCursor
        : Qt::ArrowCursor;

    if (cursor().shape() != shape) {
        setCursor(shape);
    }
}