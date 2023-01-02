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

#include "tmpwebenginepage.h"
#include "debug.h"
#include <QtGui/QDesktopServices>

TmpWebEnginePage::TmpWebEnginePage(QObject *parent)
    : QWebEnginePage(parent) {
}

TmpWebEnginePage::~TmpWebEnginePage() {
    DBUG;
}

bool TmpWebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) {
    DBUG << url << type << isMainFrame;
    deleteLater();
    if (QWebEnginePage::NavigationTypeLinkClicked==type) {
        QDesktopServices::openUrl(url);
    }
    return false;
}
