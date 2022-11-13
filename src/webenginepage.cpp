/*
 * LMS-Material-App-Qt
 *
 * Copyright (c) 2022 Craig Drummond <craig.p.drummond@gmail.com>
 *
 * ----
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "webenginepage.h"
#include "settings.h"
#include "themes.h"
#include <QtCore/QDebug>
#include <QtGui/QDesktopServices>
#include <QtWebEngineWidgets/QWebEngineProfile>

static const QLatin1String constThemeLog("MATERIAL-THEME ");

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(QWebEngineProfile::defaultProfile(), parent) {
    QObject::connect(this, &QWebEnginePage::featurePermissionRequested,
                     [&] (const QUrl &origin, QWebEnginePage::Feature feature) {
                         if (feature != QWebEnginePage::Notifications)
                             return;
                         this->setFeaturePermission(origin, feature, QWebEnginePage::PermissionGrantedByUser);
                     });
}

void WebEnginePage::setDark(bool dark) {
    setBackgroundColor((dark ? Themes::constDark.window() : Themes::constLight.window()).color());
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) {
    if (url.scheme().startsWith("mska")) {
        emit appUrl(url.toString());
        return false;
    }
    if (QWebEnginePage::NavigationTypeLinkClicked==type) {
        QDesktopServices::openUrl(url);
        return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

void WebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) {
    Q_UNUSED(level);
    Q_UNUSED(lineNumber);
    Q_UNUSED(sourceID);
    qDebug() << "MSG:" << message;
    if (message.startsWith(constThemeLog)) {
        bool dark = message.toLower().contains("dark") || message.toLower().contains("black");
        Settings cfg;
        if (dark!=cfg.getDark()) {
            cfg.setDark(dark);
            emit isDark(dark);
        }
    }
}

// TODO: Auth request? void QWebEnginePage::authenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator)
