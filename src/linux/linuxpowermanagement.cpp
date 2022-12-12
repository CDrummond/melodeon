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

#include "linuxpowermanagement.h"
#include "player.h"
#include "inhibitinterface.h"
#include "login1interface.h"
#include <QtCore/QString>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingReply>
#include <QtGui/QGuiApplication>

LinuxPowerManagement::LinuxPowerManagement(Player *p)
    : PowerManagement(p)
    , cookie(-1) {
    inhibit = new OrgFreedesktopPowerManagementInhibitInterface("org.freedesktop.PowerManagement",
                                                                QLatin1String("/org/freedesktop/PowerManagement/Inhibit"),
                                                                QDBusConnection::sessionBus(), this);
    login1 = new OrgFreedesktopLogin1ManagerInterface("org.freedesktop.login1",
                                                      QLatin1String("/org/freedesktop/login1"), QDBusConnection::systemBus(), this);
}

void LinuxPowerManagement::inhibitSuspend(bool i) {
    if (inhibitSuspendWhilstPlaying) {
        if (player->isPlaying()) {
            beginSuppressingSleep();
        }
    } else {
        stopSuppressingSleep();
    }
}

void LinuxPowerManagement::beginSuppressingSleep() {
    if (-1!=cookie || descriptor.isValid()) {
        return;
    }

    QString reason=tr("Music is playing");
    QDBusReply<uint> reply = inhibit->Inhibit(QGuiApplication::applicationDisplayName(), reason);
    cookie=reply.isValid() ? reply : -1;

    QString types=QStringLiteral("sleep");
    QString mode=QStringLiteral("block");
    QDBusPendingReply<QDBusUnixFileDescriptor> futureReply;
    futureReply = login1->Inhibit(types, QGuiApplication::applicationDisplayName(), reason, mode);
    futureReply.waitForFinished();
    if (futureReply.isValid()) {
        descriptor=futureReply.value();
    }
}

void LinuxPowerManagement::stopSuppressingSleep() {
    if (-1!=cookie) {
        inhibit->UnInhibit(cookie);
        cookie=-1;
    }

    if (descriptor.isValid()) {
        QDBusUnixFileDescriptor invalidDescriptor;
        descriptor.swap(invalidDescriptor);
    }
}

void LinuxPowerManagement::playbackStateChanged(bool playing) {
    if (inhibitSuspendWhilstPlaying) {
        if (playing) {
            beginSuppressingSleep();
        } else {
            stopSuppressingSleep();
        }
    }
}
