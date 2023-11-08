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

static const int constSpacing = 2;
static const int constButtonSize = 16;

static void configButton(QToolButton *b) {
    b->setFixedSize(constButtonSize, constButtonSize);
    b->setAutoRaise(true);
}

WindowButtons::WindowButtons(QWidget *p)
    : QWidget(p) {
    QHBoxLayout *lay = new QHBoxLayout(this);
    min = new QToolButton(this);
    max = new QToolButton(this);
    close = new QToolButton(this);

    configButton(min);
    configButton(max);
    configButton(close);

    lay->setSpacing(constSpacing);
    lay->setMargin(0);
    lay->addWidget(min);
    lay->addWidget(max);
    lay->addWidget(close);
    setLayout(lay);
    setFixedSize((3*constButtonSize) + (2*constSpacing), constButtonSize);
}

int WindowButtons::space() {
    return (3*constButtonSize) + (4*constSpacing);
}

void WindowButtons::update() {
    QSize ps = qobject_cast<QWidget *>(parent())->size();
    move(ps.width()-width(), 0);
}