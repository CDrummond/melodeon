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

#ifndef PLAYER_H
#define PLAYER_H

#include "status.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

class QNetworkAccessManager ;

class Player : public QObject {
    Q_OBJECT
public:
    Player(QObject *p);

    void handleCommand(const QByteArray &cmd);
    bool isPlaying() const { return status.playing; }

public slots:
    void update(const QString &i, const QString &n);
    void play();
    void pause();
    void playPause();
    void prev();
    void next();
    void stop();
    void seekTo(double pos);
    void setRepeat(int val);
    void setShuffle(int val);
    void setVolume(int val);
    void incrementVolume();
    void decrementVolume();
    virtual void statusUpdate(const Status &status);
    virtual void setCover(const QString &url);

signals:
    void runCommand(const QString &cmd);
    void playbackStateChanged(bool playing);

private:
    void sendCommand(const QStringList &command);

protected:
    Status status;
    QString coverUrl;

private:
    QString id;
    QString name;
    QNetworkAccessManager *mgr;
};

#endif
