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

#include "webenginepage.h"
#include "tmpwebenginepage.h"
#include "debug.h"
#include "settings.h"
#include "status.h"
#include "themes.h"
#include <QtGui/QDesktopServices>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineSettings>

static const QLatin1String constThemeLog("MATERIAL-THEME");
static const QLatin1String constStatusLog("MATERIAL-STATUS");
static const QLatin1String constCoverLog("MATERIAL-COVER");
static const QLatin1String constPlayerLog("MATERIAL-PLAYER");
static const QLatin1String constName("NAME");
static const QLatin1String constPlaying("PLAYING");
static const QLatin1String constCount("COUNT");
static const QLatin1String constTitle("TITLE");
static const QLatin1String constArtist("ARTIST");
static const QLatin1String constAlbum("ALBUM");
static const QLatin1String constDuration("DURATION");
static const QLatin1String constTrackId("TRACKID");
static const QLatin1String constTime("TIME");
static const QLatin1String constId("ID");
static const QLatin1String constUrl("URL");
static const QLatin1String constShuffle("SHUFFLE");
static const QLatin1String constRepeat("REPEAT");
static const QLatin1String constVolume("VOLUME");

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(QWebEngineProfile::defaultProfile(), parent) {
    QObject::connect(this, &QWebEnginePage::featurePermissionRequested,
                     [&] (const QUrl &origin, QWebEnginePage::Feature feature) {
                         if (feature != QWebEnginePage::Notifications)
                             return;
                         this->setFeaturePermission(origin, feature, QWebEnginePage::PermissionGrantedByUser);
                     });
    qRegisterMetaType<Status>("Status");
    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
}

void WebEnginePage::setDark(bool dark) {
    setBackgroundColor((dark ? Themes::constDark.window() : Themes::constLight.window()).color());
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) {
    DBUG << url << type << isMainFrame;
    if (url.scheme().startsWith("mska")) {
        emit appUrl(url.toString());
        return false;
    }
    if (QWebEnginePage::NavigationTypeLinkClicked==type && isMainFrame) {
        QDesktopServices::openUrl(url);
        return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

void WebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) {
    DBUG_JS << level << lineNumber << sourceID << message;

    if (message.startsWith(constThemeLog)) {
        handleTheme(parse(message));
    } else if (message.startsWith(constStatusLog)) {
        handleStatus(parse(message));
    } else if (message.startsWith(constCoverLog)) {
        handleCover(parse(message));
    } else if (message.startsWith(constPlayerLog)) {
        handlePlayer(parse(message));
    }
}

QWebEnginePage * WebEnginePage::createWindow(QWebEnginePage::WebWindowType type) {
    DBUG << type;
    if (QWebEnginePage::WebBrowserTab==type || QWebEnginePage::WebBrowserWindow==type) {
        return new TmpWebEnginePage(this);
    }
    return nullptr;
}

void WebEnginePage::runCommand(const QString &command) {
    runJavaScript(command);
}

QMap<QString, QString> WebEnginePage::parse(const QString &message) {
    QMap<QString, QString> map;
    QStringList lines = message.split("\n", Qt::SkipEmptyParts);
    for (const auto &line: lines) {
        int pos = line.indexOf(' ');
        if (pos>0 && pos<line.size()) {
            map.insert(line.left(pos), line.mid(pos+1));
        }
    }
    return map;
}

void WebEnginePage::handleTheme(const QMap<QString, QString> &params) {
    QString name = params[constName].toLower();
    bool dark = name.contains("dark") || name.contains("black");
    if (dark!=Settings::self()->getDark()) {
        Settings::self()->setDark(dark);
        emit isDark(dark);
    }
}

void WebEnginePage::handleStatus(const QMap<QString, QString> &params) {
    Status stat;
    stat.playing=params[constPlaying].toLower()=="true";
    stat.count=params[constCount].toUInt();
    stat.title=params[constTitle];
    stat.artist=params[constArtist];
    stat.album=params[constAlbum];
    stat.duration=params[constDuration].toDouble();
    stat.time=params[constTime].toDouble();
    stat.id=params[constTrackId];
    stat.shuffle=(Status::Shuffle)params[constShuffle].toUInt();
    stat.repeat=(Status::Repeat)params[constRepeat].toUInt();
    stat.volume=params[constVolume].toUInt();
    emit status(stat);
}

void WebEnginePage::handleCover(const QMap<QString, QString> &params) {
    QString url = params[constUrl];
    if (!url.isEmpty() && !url.startsWith("http")) {
        url=QLatin1String("http://%1:%2").arg(Settings::self()->getAddress()).arg(Settings::self()->getPort())+url;
    }
    emit cover(url);
}

void WebEnginePage::handlePlayer(const QMap<QString, QString> &params) {
    emit player(params[constId], params[constName]);
}

// TODO: Auth request? void QWebEnginePage::authenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator)
