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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QStackedWidget;
class QWebEngineView;
class Player;
class SettingsWidget;
class WebEnginePage;
#ifdef Q_OS_LINUX
class Mpris;
#endif

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    enum Desktop {
        Windows,
        Mac,
        KDE,
        GNOME,
        Other
    };

    explicit MainWindow();
    void closeEvent(QCloseEvent *event) override;
    void showSettings();

public slots:
    void reload();
    void zoomIn();
    void zoomOut();
    void loadFinished(bool ok);
    void stackChanged(int index);
    void appUrl(const QString &url);
    void titleChanged(const QString &title);

private slots:
    void setTheme(bool dark);
    void settingsClosed(bool clearCache);
    void timeout();

private:
    void determineDesktop();
    void setupProfile();
    void showPage(int index);
    void loadUrl(const QString &url);
    QString buildUrl();
    void setTitle();

private:
    Player *player;
#ifdef Q_OS_LINUX
    Mpris *mpris;
#endif
    Desktop desktop;
    QStackedWidget *stack;
    SettingsWidget *settings;
    WebEnginePage *page;
    QWebEngineView *web;
    bool pageLoaded;
    QString currentUrl;
    QString urlTitle;
};

#endif // MAINWINDOW_H
