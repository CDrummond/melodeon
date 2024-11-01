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
#include "edge.h"
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
#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QSize>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QKeySequence>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QWindow>
#include <QtNetwork/QAuthenticator>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWebEngineCore/QWebEngineProfile>
#else
#include <QtWebEngineWidgets/QWebEngineProfile>
#endif
#include <QtWebEngineWidgets/QWebEngineView>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif
#include <QtWidgets/QStackedWidget>

static const QString constSettingsUrl("mska://settings");
static const int constMaxCacheSize = 1024;
static const QLatin1String constUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36");

enum Pages {
    SETTINGS_PAGE = 0,
    WEBVIEW_PAGE = 1
};

qreal MainWindow::constMinZoom(0.25);
qreal MainWindow::constMaxZoom(5.0);
qreal MainWindow::constZoomStep(0.05);

static bool useConstomToolbar = false;
bool MainWindow::customWindowbar() {
    return useConstomToolbar;
}

MainWindow::MainWindow()
    : QMainWindow(nullptr) {
    useConstomToolbar = Settings::self()->getCustomTitlebar();
    wasMaximised = false;

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
    page = new WebEnginePage(profile, web);
    setTheme(Settings::self()->getDark());
    web->setPage(page);
    loadUrl(buildUrl());
    web->setContextMenuPolicy(Qt::NoContextMenu);
    connect(web, &QWebEngineView::titleChanged, this, &MainWindow::titleChanged);
    connect(web, &QWebEngineView::loadFinished, this, &MainWindow::loadFinished);
    connect(stack, &QStackedWidget::currentChanged, this, &MainWindow::stackChanged);
    connect(settings, &SettingsWidget::close, this, &MainWindow::settingsClosed);
    connect(settings, &SettingsWidget::quit, this, &MainWindow::quit);
    connect(page, &WebEnginePage::appUrl, this, &MainWindow::appUrl);
    connect(page, &WebEnginePage::isDark, this, &MainWindow::setTheme);
    connect(page, &WebEnginePage::player, player, &Player::update);
    connect(page, &WebEnginePage::status, player, &Player::statusUpdate);
    connect(page, &WebEnginePage::cover, player, &Player::setCover);
    connect(page, &QWebEnginePage::authenticationRequired, this, &MainWindow::authenticationRequired);
    connect(page, &WebEnginePage::titlebarPressed, this, &MainWindow::titlebarPressed);
    connect(player, &Player::runCommand, page, &WebEnginePage::runCommand);

    stack->addWidget(settings);
    stack->addWidget(web);
    setCentralWidget(stack);
    showPage(WEBVIEW_PAGE);

    windowSize = Settings::self()->getWindowSize();
    if (windowSize.isEmpty()) {
        QSize screenSize = qApp->screens()[0]->size();
        windowSize=QSize(qMax(screenSize.width()-64, 1024), qMax(screenSize.height()-64, 768));
    }
    resize(windowSize);

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAction *settingsAct = new QAction(this);
    settingsAct->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_Comma);
    connect(settingsAct, &QAction::triggered, this, &MainWindow::showSettings);
    addAction(settingsAct);
#endif

    setMinimumSize(450, 500);
    if (powerManagement) {
        powerManagement->setInhibitSuspend(Settings::self()->getInhibitSuspend());
        connect(powerManagement, &PowerManagement::resuming, this, &MainWindow::resuming);
    }

    if (useConstomToolbar) {
        setWindowFlags(Qt::FramelessWindowHint);
        stack->setContentsMargins(0, 0, 0, 0);
        edges[0] = new Edge(Qt::LeftEdge, 2, this);
        edges[1] = new Edge(Qt::TopEdge, 3, this);
        edges[2] = new Edge(Qt::RightEdge, 2, this);
        edges[3] = new Edge(Qt::BottomEdge, 3, this);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Settings::self()->setZoom(page->zoomFactor());
    Settings::self()->setMaximized(isMaximized());
    Settings::self()->setWindowSize(windowSize);
    Settings::self()->save();
    page->deleteLater();
    QMainWindow::closeEvent(event);
}

void MainWindow::showSettings() {
    showPage(SETTINGS_PAGE);
}

void MainWindow::reload() {
    web->stop();
    profile->clearHttpCache();
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
    DBUG << ok;
    if (ok) {
        pageLoaded = true;
        page->setZoomFactor(Settings::self()->getZoom());
        wasMaximised = isMaximized();
        page->setMaximized(wasMaximised);
        update();
    } else {
        showPage(SETTINGS_PAGE);
    }
}

void MainWindow::stackChanged(int index) {
}

void MainWindow::appUrl(const QString &url) {
    if (url==constSettingsUrl) {
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
    QTimer::singleShot(500, web, &QWebEngineView::reload);
}

void MainWindow::resuming() {
    DBUG;
    if (WEBVIEW_PAGE==stack->currentIndex()) {
        page->runCommand("refreshStatus");
        // And 1/2 second later
        QTimer::singleShot(500, [=]() {
            page->runCommand("refreshStatus");
        });
    }
}

void MainWindow::setTheme(bool dark) {
    Edge::setDark(dark);
#if defined Q_OS_WIN
    // TODO
#elif defined Q_OS_MAC
    // TODO
#else
    if (KDE==desktop) {
        qApp->setProperty("KDE_COLOR_SCHEME_PATH",
                          dark ? "/usr/share/color-schemes/BreezeDark.colors"
                               : "/usr/share/color-schemes/BreezeLight.colors");
    }
    qApp->setPalette(dark ? Themes::constDark : Themes::constLight);
    page->setDark(dark);
    settings->setDark(dark);
#endif
}

void MainWindow::settingsClosed(bool clearCache) {
    showPage(WEBVIEW_PAGE);
    page->setZoomFactor(Settings::self()->getZoom());
    if (powerManagement) {
        powerManagement->setInhibitSuspend(Settings::self()->getInhibitSuspend());
    }
    if (clearCache || !pageLoaded) {
        web->stop();
        if (clearCache) {
            profile->clearHttpCache();
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

void MainWindow::authenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator) {
    DBUG << requestUrl;
    authenticator->setUser(Settings::self()->getUsername());
    authenticator->setPassword(Settings::self()->getPassword());
}

void MainWindow::titlebarPressed(const QString &name) {
    if ("min"==name) {
        showMinimized();
    } else if ("max"==name) {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    } else if ("close"==name) {
        close();
    } else if ("move"==name) {
        windowHandle()->startSystemMove();
    }
}

void MainWindow::quit() {
    close();
}

bool MainWindow::event(QEvent *event) {
    if (useConstomToolbar && (QEvent::Resize==event->type() || QEvent::WindowStateChange==event->type())) {
        edges[0]->update();
        edges[1]->update();
        edges[2]->update();
        edges[3]->update();
        bool nowMaximized = isMaximized();
        if (!nowMaximized) {
            windowSize = size();
        }
        if (nowMaximized!=wasMaximised) {
            wasMaximised = nowMaximized;
            page->setMaximized(wasMaximised);
        }
    }
    return QWidget::event(event);
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
    profile = new QWebEngineProfile("Melodeon", this);
    profile->setHttpCacheMaximumSize(constMaxCacheSize*1024*1024);
    profile->setHttpUserAgent(constUserAgent);
    profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
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
                  QLatin1String("/material/?hide=mediaControls,scale&download=false&nativeTheme=c&dontTrapBack") +
#ifdef Q_OS_LINUX
                  QLatin1String("&nativeStatus=c&nativePlayer=c&nativeCover=c") +
#endif
                  QLatin1String("&appSettings=")+constSettingsUrl;
    if (useConstomToolbar) {
        url+="&nativeTitlebar=c";
    }
    if (KDE==desktop || Windows==desktop) {
        url+=QLatin1String("&altBtnLayout=true");
    }
    if (KDE==desktop) {
        url+=QLatin1String("&defaultTheme=linux/dark/Breeze-Dark&themeColor=2a2e32");
    } else if (GNOME==desktop) {
        if (useConstomToolbar) {
            url+=QLatin1String("&defaultTheme=linux/dark/Adwaita-Dark&themeColor=2c2c2c");
        } else {
            url+=QLatin1String("&defaultTheme=linux/light/Adwaita&themeColor=fcfcfc");
        }
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
