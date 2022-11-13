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
#include "serverdiscovery.h"
#include "settings.h"
#include "startup.h"
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

Startup::Startup()
    : QDialog(nullptr) {
    setWindowTitle(tr("Server Discovery"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Attempting to auto-discover an LMS instance on your network..."), this));
    layout->addWidget(buttonBox);
    setLayout(layout);
    setModal(false);

    discovery = new ServerDiscovery(this);
    connect(discovery, &ServerDiscovery::server, this, &Startup::serverDiscovered);
    connect(discovery, &ServerDiscovery::finished, this, &Startup::discoverFinished);
    discovery->start();
}

void Startup::serverDiscovered(const QString &name, const QString &addr, quint16 port) {
    if (!discovery) {
        return;
    }
    Settings cfg;
    cfg.setName(name);
    cfg.setAddress(addr);
    cfg.setPort(port);
    cfg.save();
    showMainWindow(false);
}

void Startup::discoverFinished() {
    if (!discovery) {
        return;
    }
    showMainWindow(true);
}

void Startup::showMainWindow(bool withSettings) {
    disconnect(discovery, &ServerDiscovery::server, this, &Startup::serverDiscovered);
    disconnect(discovery, &ServerDiscovery::finished, this, &Startup::discoverFinished);
    discovery->stop();
    discovery = nullptr;
    MainWindow *mw = new MainWindow();
    mw->show();
    if (withSettings) {
        mw->showSettings();
    }
    accept();
    deleteLater();
}
