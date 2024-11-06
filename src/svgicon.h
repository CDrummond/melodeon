/*
 * Melodeon - LMS Controller
 *
 * Copyright (c) 2022-2024 Craig Drummond <craig.p.drummond@gmail.com>
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

#ifndef SVG_ICON_H
#define SVG_ICON_H

#include <QtGui/QColor>
#include <QtGui/QIcon>

namespace SvgIcon {
    extern QIcon icon(const QString &fileName, const QColor &col, const QColor &sel=QColor(QColor::Invalid));
}

#endif // SVG_ICON_H
