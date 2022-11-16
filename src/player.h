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

#ifndef PLAYER_H
#define PLAYER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

class QNetworkAccessManager ;

class Player : public QObject {
    Q_OBJECT
public:
    Player(QObject *p);

public slots:
    void update(const QString &i, const QString &n);
    void play();
    void pause();
    void prev();
    void next();
    void stop();
    void seekTo(double pos);
    void setRepeat(int val);
    void setShuffle(int val);
    void setVolume(int val);

signals:
    void updateStatus();

private:
    void sendCommand(const QStringList &command);

private:
    QString id;
    QString name;
    QNetworkAccessManager  *mgr;
};

#endif
