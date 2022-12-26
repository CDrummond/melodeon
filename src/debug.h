/*
 * Melodeon - LMS Controller
 *
 * Copyright (c) 2022 Craig Drummond <craig.p.drummond@gmail.com>
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

#ifndef DEBUG_H
#define DEBUG_H

#include <QtCore/QDebug>
#include <QtCore/QStringList>

namespace Debug {
enum Areas {
    None   = 0x00,
    App    = 0x01,
    JSON   = 0x02,
    CometD = 0x04
};

extern int areas;
extern void init(const QStringList &rgs);
}

#define DBUG_CLASS(CLASS) if (Debug::areas&Debug::App) qDebug() << CLASS << __FUNCTION__
#define DBUG DBUG_CLASS(metaObject()->className())
#define DBUG_JS if (Debug::areas&(Debug::JSON|Debug::CometD)) qDebug() << metaObject()->className() << __FUNCTION__

#endif
