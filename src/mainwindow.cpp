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

#include "mainwindow.h"
#include "settings.h"
#include "settingswidget.h"
#include "themes.h"
#include "webenginepage.h"
#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QSize>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QKeySequence>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWidgets/QAction>
#include <QtWidgets/QStackedWidget>

const QString constSettingsUrl("mska://settings");
const QString constQuitUrl("mska://quit");

enum Pages {
    SETTINGS_PAGE = 0,
    WEBVIEW_PAGE = 1
};

MainWindow::MainWindow()
    : QMainWindow(nullptr) {
    qDebug() << "CREATE";
    pageLoaded = false;
    determineDesktop();
    stack = new QStackedWidget(this);
    Settings cfg;
    settings = new SettingsWidget(this);
    web = new QWebEngineView(stack);
    page = new WebEnginePage(web);
    setTheme(cfg.getDark());
    web->setPage(page);
    loadUrl(buildUrl());
    web->setContextMenuPolicy(Qt::NoContextMenu);
    connect(web, &QWebEngineView::titleChanged, this, &MainWindow::titleChanged);
    connect(web, &QWebEngineView::loadFinished, this, &MainWindow::loadFinished);
    connect(stack, &QStackedWidget::currentChanged, this, &MainWindow::stackChanged);
    connect(page, &WebEnginePage::appUrl, this, &MainWindow::appUrl);
    connect(page, &WebEnginePage::isDark, this, &MainWindow::setTheme);
    connect(settings, &SettingsWidget::close, this, &MainWindow::settingsClosed);
    stack->addWidget(settings);
    stack->addWidget(web);
    setCentralWidget(stack);
    showPage(WEBVIEW_PAGE);

    QByteArray geo = cfg.getGeometry();
    if (geo.isEmpty()) {
        QSize screenSize = qApp->screens()[0]->size();
        resize(qMin(screenSize.width()-64, 1024), qMin(screenSize.height()-64, 768));
    } else {
        restoreGeometry(geo);
    }

    QByteArray state = cfg.getState();
    if (!state.isEmpty()) {
        restoreGeometry(state);
    }

    web->setZoomFactor(cfg.getZoom());

    QAction *reloadAct = new QAction(this);
    reloadAct->setShortcut(QKeySequence::Refresh);
    connect(reloadAct, &QAction::triggered, this, &MainWindow::reload);
    addAction(reloadAct);

    QAction *zoomInAct = new QAction(this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);
    addAction(zoomInAct);

    QAction *zoomOutAct = new QAction(this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);
    addAction(zoomOutAct);
    setMinimumSize(450, 500);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Settings cfg;
    cfg.setGeometry(saveGeometry());
    cfg.setState(saveState());
    cfg.setZoom(web->zoomFactor());
    cfg.save();
    QMainWindow::closeEvent(event);
}

void MainWindow::showSettings() {
    showPage(SETTINGS_PAGE);
}

void MainWindow::reload() {
    web->stop();
    QWebEngineProfile::defaultProfile()->clearHttpCache();
    QTimer::singleShot(500, web, &QWebEngineView::reload);
}

void MainWindow::zoomIn() {
    qreal zoom = page->zoomFactor();
    if (zoom<=4.75) {
        zoom += 0.25;
        page->setZoomFactor(zoom);
    }
}

void MainWindow::zoomOut() {
    qreal zoom = page->zoomFactor();
    if (zoom>=0.5) {
        zoom -= 0.25;
        page->setZoomFactor(zoom);
    }
}

void MainWindow::loadFinished(bool ok) {
    qDebug() << "loadFinished" << ok;
    if (ok) {
        pageLoaded = true;
    } else {
        showPage(SETTINGS_PAGE);
    }
}

void MainWindow::stackChanged(int index) {
}

void MainWindow::appUrl(const QString &url) {
    if (url==constQuitUrl) {
        close();
    } else if (url==constSettingsUrl) {
        showPage(SETTINGS_PAGE);
    }
}

void MainWindow::titleChanged(const QString &title) {
    urlTitle = title.contains("Logitech") ? title : QString();
    if (WEBVIEW_PAGE==stack->currentIndex()) {
        setWindowTitle(!urlTitle.isEmpty() ? urlTitle : "LMS");
    }
}

void MainWindow::setTheme(bool dark) {
#if defined Q_OS_WIN
    // TODO
#elif defined Q_OS_MAC
    // TODO
#else
    qApp->setProperty("KDE_COLOR_SCHEME_PATH",
                      dark ? "/usr/share/color-schemes/BreezeDark.colors"
                           : "/usr/share/color-schemes/BreezeLight.colors");
#endif
    qApp->setPalette(dark ? Themes::constDark : Themes::constLight);
    page->setDark(dark);
    settings->setDark(dark);
}

void MainWindow::settingsClosed(bool clearCache) {
    showPage(WEBVIEW_PAGE);
    web->setZoomFactor(Settings().getZoom());
    if (clearCache || !pageLoaded) {
        web->stop();
        QWebEngineProfile::defaultProfile()->clearHttpCache();
        QString url = buildUrl();
        if (url!=currentUrl) {
            loadUrl(url);
        } else {
            QTimer::singleShot(500, web, &QWebEngineView::reload);
        }
    } else {
        QString url = buildUrl();
        if (url!=currentUrl) {
            loadUrl(url);
        }
    }
}

void MainWindow::timeout() {
    if (!pageLoaded) {
        web->stop();
        showPage(SETTINGS_PAGE);
    }
}

void MainWindow::determineDesktop() {
#if defined Q_OS_WIN
    desktop=Windows;
#elif defined Q_OS_MAC
    desktop=Mac;
#else
    desktop=Other;
    QList<QByteArray> xdgDesktop = qgetenv("XDG_CURRENT_DESKTOP").toLower().split(':');
    QSet<QByteArray> de = QSet<QByteArray>(xdgDesktop.cbegin(), xdgDesktop.cend());
    if (de.contains("kde")) {
        desktop=KDE;
    } else if (de.contains("gnome") || de.contains("pantheon")) {
        desktop=GNOME;
    }
#endif
}

void MainWindow::showPage(int index) {
    qDebug() << "showPage" << index << stack->currentIndex();
    if (index==stack->currentIndex()) {
        return;
    }
    stack->setCurrentIndex(index);
    setWindowTitle(WEBVIEW_PAGE==index && !urlTitle.isEmpty() ? urlTitle : "LMS");
    if (SETTINGS_PAGE==index) {
        settings->update();
    }
}

void MainWindow::loadUrl(const QString &url) {
    currentUrl = url;
    pageLoaded = false;
    web->load(QUrl(currentUrl));
    QTimer::singleShot(10000, this, &MainWindow::timeout);
}

QString MainWindow::buildUrl() {
    Settings cfg;
    QString url = QLatin1String("http://") +
                  cfg.getAddress() +
                  QLatin1Char(':') +
                  QString::number(cfg.getPort()) +
                  QLatin1String("/material/?hide=notif,scale&download=native&nativeTheme=c") +
                  QLatin1String("&appSettings=")+constSettingsUrl +
                  QLatin1String("&appQuit=")+constQuitUrl;
    if (KDE==desktop || Windows==desktop) {
        url+=QLatin1String("&altBtnLayout=true");
    }
    qDebug() << url;
    return url;
}
