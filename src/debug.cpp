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

#include "debug.h"
#include <QtCore/QDateTime>

int Debug::areas = Debug::None;

void Debug::init(const QStringList &args) {
    for (const auto &arg: args) {
        if (arg.startsWith("--debug")) {
            QStringList parts = args.at(1).mid(8).split(",");
            for (const auto &part: parts) {
                if (part=="app") {
                    areas|=Debug::App;
                } else if (part=="json") {
                    areas|=Debug::JSON;
                } else if (part=="cometd") {
                    areas|=Debug::CometD;
                }
            }
            break;
        }
    }
}

QString Debug::prefix(const QString &clz, const QString &func) {
    return "[" + QDateTime::currentDateTime().toString("dd.MM.yy hh.mm.ss.z") + " | " + clz + " | " + func + "]";
}
