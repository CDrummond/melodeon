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

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "columnresizer.h"
#include "mainwindow.h"
#include "serverdiscovery.h"
#include "settings.h"
#include <QtGui/QColor>
#include <QtGui/QFontMetrics>
#include <QtWidgets/QAction>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
    , discovery(nullptr) {
    ui->setupUi(this);
    ColumnResizer* resizer = new ColumnResizer(this);
    resizer->addWidgetsFromLayout(ui->serverGroupBox->layout(), 0);
    resizer->addWidgetsFromLayout(ui->interfaceGroupBox->layout(), 0);
    clearCache = false;
    connect(ui->backButton, &QPushButton::clicked, this, &SettingsWidget::backClicked);
    connect(ui->discoverServer, &QPushButton::clicked, this, &SettingsWidget::discoverClicked);
    connect(ui->clearCache, &QPushButton::clicked, this, &SettingsWidget::clearCacheClicked);
    connect(ui->zoom, &QSlider::valueChanged, this, &SettingsWidget::updateZoomPc);
    ui->backButton->setText(QString("\u2190"));
    QFont f = font();
    f.setBold(true);
    ui->backButton->setFont(f);
    ui->backButton->setMaximumWidth(ui->toolbar->height()+4);
    ui->backButton->setMinimumHeight(ui->toolbar->height());

    f = font();
    f.setFixedPitch(true);
    QFontMetrics fm(f);
    ui->zoomPc->setFont(f);
    ui->zoomPc->setFixedWidth(fm.boundingRect("1000 % ").width());

    ui->zoom->setMinimum(0);
    ui->zoom->setMaximum((MainWindow::constMaxZoom-MainWindow::constMinZoom)/MainWindow::constZoomStep);
    ui->zoom->setSingleStep(1);

    QAction *closeAct = new QAction(this);
    closeAct->setShortcut(Qt::Key_Escape);
    connect(closeAct, &QAction::triggered, this, &SettingsWidget::backClicked);
    addAction(closeAct);
}

void SettingsWidget::setDark(bool dark) {
    QPalette pal(palette());
    pal.setColor(QPalette::Window, dark ? QColor(20, 20, 20) : QColor(220, 220, 220));
    pal.setColor(QPalette::Button, pal.color(QPalette::Button));
    ui->toolbar->setPalette(pal);
    ui->toolbar->setBackgroundRole(QPalette::Window);
    ui->backButton->setPalette(pal);
    ui->backButton->setBackgroundRole(QPalette::Window);
}

void SettingsWidget::backClicked() {
    Settings::self()->setZoom(ui->zoom->value()*MainWindow::constZoomStep);
    Settings::self()->setName(ui->serverName->text().trimmed());
    Settings::self()->setAddress(ui->serverAddress->text().trimmed());
    Settings::self()->setPort(ui->serverPort->value());
    Settings::self()->save();
    emit close(clearCache);
    clearCache = false;
}

void SettingsWidget::discoverClicked() {
    if (nullptr==discovery) {
        discovery = new ServerDiscovery(this);
        connect(discovery, &ServerDiscovery::server, this, &SettingsWidget::serverDiscovered);
    }
    discovery->start();
}

void SettingsWidget::clearCacheClicked() {
    clearCache = true;
}

void SettingsWidget::serverDiscovered(const QString &name, const QString &addr, quint16 port) {
    if (addr!=ui->serverAddress->text().trimmed() || port!=ui->serverPort->value()) {
        if (nullptr!=discovery) {
            discovery->stop();
        }
        ui->serverName->setText(name);
        ui->serverAddress->setText(addr);
        ui->serverPort->setValue(port);
    }
}

void SettingsWidget::updateZoomPc(int val) {
    ui->zoomPc->setText(tr("%1 %").arg(qRound((MainWindow::constMinZoom+(val*MainWindow::constZoomStep))*100.0)));
}

void SettingsWidget::update() {
    clearCache = false;
    ui->zoom->setValue((int)((Settings::self()->getZoom()-MainWindow::constMinZoom)/MainWindow::constZoomStep));
    ui->serverName->setText(Settings::self()->getName());
    ui->serverAddress->setText(Settings::self()->getAddress());
    ui->serverPort->setValue(Settings::self()->getPort());
    updateZoomPc(ui->zoom->value());
}
