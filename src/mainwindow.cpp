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

#include "mainwindow.h"
#include "debug.h"
#include "player.h"
#include "settings.h"
#include "settingswidget.h"
#include "status.h"
#include "themes.h"
#include "webenginepage.h"
#ifdef Q_OS_LINUX
#include "mpris.h"
#include "linuxpowermanagement.h"
#endif
#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
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

static const QString constSettingsUrl("mska://settings");
static const QString constQuitUrl("mska://quit");
static const int constMaxCacheSize = 1024;
static const QLatin1String constUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36");

enum Pages {
    SETTINGS_PAGE = 0,
    WEBVIEW_PAGE = 1
};

qreal MainWindow::constMinZoom(0.25);
qreal MainWindow::constMaxZoom(5.0);
qreal MainWindow::constZoomStep(0.05);

MainWindow::MainWindow()
    : QMainWindow(nullptr) {
#ifdef Q_OS_LINUX
    player = new Mpris(this);
    powerManagement = new LinuxPowerManagement(player);
#else
    player = new Player(this);
    powerManagement = nullptr;
#endif
    pageLoaded = false;
    determineDesktop();
    setupProfile();
    stack = new QStackedWidget(this);
    settings = new SettingsWidget(this);
    web = new QWebEngineView(stack);
    page = new WebEnginePage(web);
    setTheme(Settings::self()->getDark());
    web->setPage(page);
    loadUrl(buildUrl());
    web->setContextMenuPolicy(Qt::NoContextMenu);
    connect(web, &QWebEngineView::titleChanged, this, &MainWindow::titleChanged);
    connect(web, &QWebEngineView::loadFinished, this, &MainWindow::loadFinished);
    connect(stack, &QStackedWidget::currentChanged, this, &MainWindow::stackChanged);
    connect(settings, &SettingsWidget::close, this, &MainWindow::settingsClosed);
    connect(page, &WebEnginePage::appUrl, this, &MainWindow::appUrl);
    connect(page, &WebEnginePage::isDark, this, &MainWindow::setTheme);
    connect(page, &WebEnginePage::player, player, &Player::update);
    connect(page, &WebEnginePage::status, player, &Player::statusUpdate);
    connect(page, &WebEnginePage::cover, player, &Player::setCover);
    connect(player, &Player::runCommand, page, &WebEnginePage::runCommand);

    stack->addWidget(settings);
    stack->addWidget(web);
    setCentralWidget(stack);
    showPage(WEBVIEW_PAGE);

    QByteArray geo = Settings::self()->getGeometry();
    if (geo.isEmpty()) {
        QSize screenSize = qApp->screens()[0]->size();
        resize(qMin(screenSize.width()-64, 1024), qMin(screenSize.height()-64, 768));
    } else {
        restoreGeometry(geo);
    }

    QByteArray state = Settings::self()->getState();
    if (!state.isEmpty()) {
        restoreGeometry(state);
    }

    web->setZoomFactor(Settings::self()->getZoom());

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

    QAction *settingsAct = new QAction(this);
    settingsAct->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_Comma);
    connect(settingsAct, &QAction::triggered, this, &MainWindow::showSettings);
    addAction(settingsAct);

    setMinimumSize(450, 500);
    if (powerManagement) {
        powerManagement->setInhibitSuspend(Settings::self()->getInhibitSuspend());
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Settings::self()->setGeometry(saveGeometry());
    Settings::self()->setState(saveState());
    Settings::self()->setZoom(web->zoomFactor());
    Settings::self()->save();
    page->deleteLater();
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
    if (zoom<=(constMaxZoom-constZoomStep)) {
        setZoom(zoom + constZoomStep);
    }
}

void MainWindow::zoomOut() {
    qreal zoom = page->zoomFactor();
    if (zoom>=(constMinZoom+constZoomStep)) {
        setZoom(zoom - constZoomStep);
    }
}

void MainWindow::loadFinished(bool ok) {
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
    setTitle();
}

void MainWindow::receivedMessage(quint32 instanceId, QByteArray message) {
    DBUG << message;
    player->handleCommand(message);
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
    web->setZoomFactor(Settings::self()->getZoom());
    if (powerManagement) {
        powerManagement->setInhibitSuspend(Settings::self()->getInhibitSuspend());
    }
    if (clearCache || !pageLoaded) {
        web->stop();
        if (clearCache) {
            QWebEngineProfile::defaultProfile()->clearHttpCache();
        }
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

void MainWindow::setupProfile() {
    QWebEngineProfile::defaultProfile()->setHttpCacheMaximumSize(constMaxCacheSize*1024*1024);
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(constUserAgent);
}

void MainWindow::showPage(int index) {
    if (index==stack->currentIndex()) {
        return;
    }
    stack->setCurrentIndex(index);
    setTitle();
    if (SETTINGS_PAGE==index) {
        settings->update();
    }
}

void MainWindow::loadUrl(const QString &url) {
    currentUrl = url;
    pageLoaded = false;
    DBUG << url;
    web->load(QUrl(currentUrl));
    QTimer::singleShot(10000, this, &MainWindow::timeout);
}

QString MainWindow::buildUrl() {
    QString url = QLatin1String("http://") +
                  Settings::self()->getAddress() +
                  QLatin1Char(':') +
                  QString::number(Settings::self()->getPort()) +
                  QLatin1String("/material/?hide=mediaControls,scale&download=false&nativeTheme=c") +
#ifdef Q_OS_LINUX
                  QLatin1String("&nativeStatus=c&nativePlayer=c&nativeCover=c") +
#endif
                  QLatin1String("&appSettings=")+constSettingsUrl +
                  QLatin1String("&appQuit=")+constQuitUrl;
    if (KDE==desktop || Windows==desktop) {
        url+=QLatin1String("&altBtnLayout=true");
    }
    if (KDE==desktop) {
        url+=QLatin1String("&desktop=KDE");
    }
    if ((Debug::areas&Debug::JSON) || (Debug::areas&Debug::CometD)) {
        url+=QLatin1String("&debug=");
        if (Debug::areas&Debug::JSON) {
            url+=QLatin1String("json");
            if (Debug::areas&Debug::CometD) {
                url+=QLatin1String(",cometd");
            }
        } else {
            url+=QLatin1String("cometd");
        }
    }
    DBUG << url;
    return url;
}

void MainWindow::setTitle() {
    QString title = WEBVIEW_PAGE==stack->currentIndex() && !urlTitle.isEmpty() ? urlTitle : "Melodeon";
    if (!Settings::self()->getName().isEmpty()) {
        title+=" (" + Settings::self()->getName() + ")";
    }
    setWindowTitle(title);
}

void MainWindow::setZoom(qreal zoom) {
    QString zoomStr = tr("Zoom: %1 %").arg(qRound(zoom*100.0));
    page->setZoomFactor(zoom);
    page->runJavaScript(QLatin1String("bus.$emit('showMessage', '%1')").arg(zoomStr));
    Settings::self()->setZoom(zoom);
}
