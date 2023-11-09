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

#include "windowbuttons.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtGui/QPalette>

static const int constSpacing = 2;
static const int constButtonSize = 16;

static void configButton(QToolButton *b, const QColor &color, const char *text) {
    b->setFixedSize(constButtonSize*2, constButtonSize);
    b->setAutoRaise(true);
    b->setText(text);
    QPalette pal = b->palette();
    pal.setColor(QPalette::Button, color);
    b->setPalette(pal);
}

WindowButtons::WindowButtons(QWidget *p)
    : QWidget(p) {
    QHBoxLayout *lay = new QHBoxLayout(this);
    min = new QToolButton(this);
    max = new QToolButton(this);
    close = new QToolButton(this);

    configButton(min, QColor(255, 153, 0), "-");
    configButton(max, QColor(51, 204, 51), "+");
    configButton(close, QColor(255, 12, 12), "x");

    lay->setSpacing(constSpacing);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    lay->setMargin(0);
#else
    lay->setContentsMargins(0, 0, 0, 0);
#endif
    lay->addWidget(min);
    lay->addWidget(max);
    lay->addWidget(close);
    setLayout(lay);
    setFixedSize((3*constButtonSize*2) + (2*constSpacing), constButtonSize);
    connect(min, &QToolButton::clicked, this, &WindowButtons::minimizeWindow);
    connect(max, &QToolButton::clicked, this, &WindowButtons::maximizeWindow);
    connect(close, &QToolButton::clicked, this, &WindowButtons::closeWindow);
}

void WindowButtons::update() {
    QSize ps = qobject_cast<QWidget *>(parent())->size();
    move(ps.width()-width(), 0);
}

void WindowButtons::minimizeWindow() {
    qobject_cast<QWidget *>(parent())->showMinimized();
}

void WindowButtons::maximizeWindow() {
    QWidget *p = qobject_cast<QWidget *>(parent());
    if (p->isMaximized()) {
        p->showNormal();
    } else {
        p->showMaximized();
    }
}

void WindowButtons::closeWindow() {
    qobject_cast<QWidget *>(parent())->close();
}