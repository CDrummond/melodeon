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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QtWidgets/QWidget>

class ServerDiscovery;
namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    SettingsWidget(QWidget *parent);

    void setDark(bool dark);
    void update();

private slots:
    void backClicked();
    void discoverClicked();
    void clearCacheClicked();
    void serverDiscovered(const QString &name, const QString &addr, quint16 port);
    void updateZoomPc(int val);

signals:
    void close(bool clearCache);

private:
    Ui::SettingsWidget *ui;
    bool clearCache;
    ServerDiscovery *discovery;
};

#endif
