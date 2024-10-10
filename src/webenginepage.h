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

#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include <QtCore/QMap>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWebEngineCore/QWebEnginePage>
#else
#include <QtWebEngineWidgets/QWebEnginePage>
#endif


class Status;

class WebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit WebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);

    void setDark(bool dark);
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;
    QWebEnginePage *createWindow(WebWindowType type) override;

    void setMaximized(bool isMaximized);

public slots:
    void runCommand(const QString &command);

signals:
    void appUrl(const QString &url);
    void isDark(bool dark);
    void player(const QString &id, const QString &name);
    void status(const Status &status);
    void cover(const QString &url);
    void titlebarPressed(const QString &name);

private:
    QMap<QString, QString> parse(const QString &message);
    void handleTheme(const QMap<QString, QString> &params);
    void handleStatus(const QMap<QString, QString> &params);
    void handleCover(const QMap<QString, QString> &params);
    void handlePlayer(const QMap<QString, QString> &params);
    void handleTitlebar(const QMap<QString, QString> &params);
};

#endif // WEBENGINEPAGE_H
