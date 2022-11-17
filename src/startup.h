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

#ifndef STARTUP_H
#define STARTUP_H

#include <QtWidgets/QDialog>

class ServerDiscovery;

class Startup : public QDialog {
    Q_OBJECT
public:
    Startup();
private slots:
    void serverDiscovered(const QString &name, const QString &addr, quint16 port);
    void discoverFinished();
private:
    void showMainWindow(bool withSettings);
private:
    ServerDiscovery *discovery;
};

#endif
