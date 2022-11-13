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

#include "serverdiscovery.h"
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkDatagram>
#include <QtNetwork/QUdpSocket>

static QString getString(const QByteArray &data, int start, int len) {
    QString str;
    for (int i=0; i<len; ++i) {
        str+=QChar(data[start+i]);
    }
    return str;
}

ServerDiscovery::ServerDiscovery(QObject *p)
    : QObject(p)
    , socket(nullptr) {
}

void ServerDiscovery::start() {
    if (nullptr==socket) {
        socket = new QUdpSocket(this);
        connect(socket, &QUdpSocket::readyRead, this, &ServerDiscovery::readPendingDatagrams);
        char data[] = { 'e', 'I', 'P', 'A', 'D', 0, 'N', 'A', 'M', 'E', 0, 'J', 'S', 'O', 'N', 0 };
        QNetworkDatagram datagram(QByteArray(data, 16), QHostAddress::Broadcast, 3483);
        QTimer::singleShot(1500, this, &ServerDiscovery::stop);
        socket->writeDatagram(datagram);
    }
}

void ServerDiscovery::stop() {
    if (nullptr!=socket) {
        disconnect(socket, nullptr, nullptr, nullptr);
        socket->abort();
        socket->deleteLater();
        socket = nullptr;
        emit finished();
    }
}

void ServerDiscovery::readPendingDatagrams() {
    if (nullptr!=socket && socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QString addr = QHostAddress(datagram.senderAddress().toIPv4Address()).toString();
        QString name;
        QString port;
        QByteArray data = datagram.data();
        for(int i=1; i < data.length() && (name.isEmpty() || port.isEmpty());) {
            if (i + 5 > data.length()) {
                break;
            }
            QString key = getString(data, i, 4);
            int len = data[i+4]&0xff;
            i += 5;
            if (i+len > data.length()) {
                break;
            }
            if (key==QLatin1String("NAME")) {
                name = getString(data, i, len);
            } else if (key==QLatin1String("JSON")) {
                port = getString(data, i, len);
            }
            i += len;
        }
        if (!name.isEmpty() && !addr.isEmpty() && !port.isEmpty()) {
            emit server(name, addr, port.toUInt());
        }
    }
}
