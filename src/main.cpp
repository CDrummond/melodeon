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

#include "config.h"
#include "mainwindow.h"
#include "settings.h"
#include "startup.h"
#include <QtCore/QCoreApplication>
#include <singleapplication.h>

int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName(PROJECT_NAME);
    QCoreApplication::setApplicationName(PROJECT_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    SingleApplication app(argc, argv);

    if (Settings::self()->getAddress().isEmpty()) {
        Startup *start = new Startup();
        QObject::connect( &app, &SingleApplication::instanceStarted, [ start ]() {
            start->raise();
            start->activateWindow();
        });
        start->show();
    } else {
        MainWindow *mw = new MainWindow();
        QObject::connect( &app, &SingleApplication::instanceStarted, [ mw ]() {
            mw->raise();
            mw->activateWindow();
        });
        mw->show();
    }

    return app.exec();
}
