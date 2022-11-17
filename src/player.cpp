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

#include "player.h"
#include "settings.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

Player::Player(QObject *p)
    : QObject(p)
    , mgr(nullptr) {
}

void Player::update(const QString &i, const QString &n) {
    id = i;
    name = n;
}

void Player::play() {
    sendCommand(QStringList() << "play");
}

void Player::pause() {
    sendCommand(QStringList() << "pause" << "1");
}

void Player::prev() {
    sendCommand(QStringList() << "button" << "jump_rew");
}

void Player::next() {
    sendCommand(QStringList() << "playlist" << "index" << "+1");
}

void Player::stop() {
    sendCommand(QStringList() << "stop");
}

void Player::seekTo(double pos) {
    sendCommand(QStringList() << "time" << QString::number(pos));
}

void Player::setRepeat(int val) {
    sendCommand(QStringList() << "playlist" << "repeat" << QString::number(val));
}

void Player::setShuffle(int val) {
    sendCommand(QStringList() << "playlist" << "shuffle" << QString::number(val));
}

void Player::setVolume(int val) {
    sendCommand(QStringList() << "mixer" << "volume" << QString::number(val));
}

void Player::sendCommand(const QStringList &command) {
    if (id.isEmpty()) {
        return;
    }

    QJsonObject req;
    QJsonArray params;
    QJsonArray cmd;
    params.append(id);
    for (const auto& p: command) {
        cmd.append(p);
    }
    params.append(cmd);
    req.insert("id", 1);
    req.insert("method", "slim.request");
    req.insert("params", params);

    if (!mgr) {
        mgr = new QNetworkAccessManager(this);
    }

    const QUrl url(QStringLiteral("http://%1:%2/jsonrpc.js").arg(Settings::self()->getAddress()).arg(Settings::self()->getPort()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QByteArray data = QJsonDocument(req).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = mgr->post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        emit updateStatus();
        reply->deleteLater();
    });
}
