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

#include "mpris.h"
#include "config.h"
#include "playeradaptor.h"
#include "rootadaptor.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

static inline qlonglong secondsToMicroseconds(double t) {
    return t*1000000;
}

static inline double microsecondsToSeconds(qlonglong t) {
    return t/1000000.0;
}

Mpris::Mpris(Player *p)
    : QObject(p)
    , player(p) {
    new PlayerAdaptor(this);
    new MediaPlayer2Adaptor(this);

    QDBusConnection::sessionBus().registerService(QLatin1String("org.mpris.MediaPlayer2.%1").arg(PROJECT_NAME));
    QDBusConnection::sessionBus().registerObject("/org/mpris/MediaPlayer2", this, QDBusConnection::ExportAdaptors);
}

QString Mpris::PlaybackStatus() const {
    if (status.count<=0) {
        return QLatin1String("Stopped");
    }

    if (status.playing) {
        return QLatin1String("Playing");
    } else if (status.time>0 || status.duration<=0) {
        return QLatin1String("Paused");
    } else {
        return QLatin1String("Stopped");
    }
}

QString Mpris::LoopStatus() {
    switch (status.repeat) {
    case Status::RepeatOff: return QLatin1String("None");
    case Status::RepeatSingle: return QLatin1String("Track");
    default: return QLatin1String("Playlist");
    }
}

void Mpris::SetLoopStatus(const QString &s) {
    player->setRepeat(s=="None" ? Status::RepeatOff : s=="Track" ? Status::RepeatSingle : Status::RepeatAll);
}

void Mpris::SetShuffle(bool s) {
    player->setShuffle(s ? Status::ShufflePlaylist : Status::ShuffleOff);
}

void Mpris::Seek(qlonglong pos) {
    player->seekTo(microsecondsToSeconds(pos));
}

void Mpris::SetPosition(const QDBusObjectPath &trackId, qlonglong pos) {
    if (currentTrackId()==trackId.path()) {
        Seek(pos);
    }
}

qlonglong Mpris::Position() {
    return secondsToMicroseconds(status.time);
}

QVariantMap Mpris::Metadata() const {
    QVariantMap metadataMap;

    if (!status.id.isEmpty()) {
        metadataMap.insert("mpris:trackid", currentTrackId());

        if (status.duration>0) {
            metadataMap.insert("mpris:length", secondsToMicroseconds(status.duration));
        }
        if (!status.album.isEmpty()) {
            metadataMap.insert("xesam:album", status.album);
        }
        if (!status.artist.isEmpty()) {
            metadataMap.insert("xesam:artist", QStringList() << status.artist);
        }
        if (!status.title.isEmpty()) {
            metadataMap.insert("xesam:title", status.title);
        }
        if (!coverUrl.isEmpty()) {
            metadataMap.insert("mpris:artUrl", coverUrl);
        }
    }

    return metadataMap;
}

void Mpris::statusUpdate(const Status &stat) {
    QVariantMap map;
    Status prevStatus = status;

    status = stat;
    if (stat.repeat!=prevStatus.repeat) {
         map.insert("LoopStatus", LoopStatus());
    }
    if (stat.shuffle!=prevStatus.shuffle) {
         map.insert("Shuffle", Shuffle());
    }
    if (stat.volume!=prevStatus.volume) {
         map.insert("Volume", Volume());
    }
    if (stat.count!=prevStatus.count) {
        map.insert("CanGoPrevious", CanGoPrevious());
        map.insert("CanGoNext", CanGoNext());
    }
    if (stat.count!=prevStatus.count || stat.playing!=prevStatus.playing) {
        map.insert("PlaybackStatus", PlaybackStatus());
        map.insert("CanPause", CanPause());
        map.insert("CanPlay", CanPlay());
    }
    if (stat.id!=stat.id || stat.duration!=prevStatus.duration) {
        map.insert("CanSeek", CanSeek());
    }
    if (stat.time!=status.time) {
        map.insert("Position", secondsToMicroseconds(stat.time));
    }
    if (!map.isEmpty() || stat.title!=prevStatus.title || stat.artist!=prevStatus.artist ||
        stat.album!=prevStatus.album || stat.duration!=prevStatus.duration) {
        if (!map.contains("Position")) {
            map.insert("Position", secondsToMicroseconds(stat.time));
        }
        map.insert("Metadata", Metadata());
        signalUpdate(map);
    }
    status = stat;
}

void Mpris::setCover(const QString &url) {
    qWarning() << url;
    if (url==coverUrl) {
        return;
    }
    coverUrl = url;
    signalUpdate("Metadata", Metadata());
}

void Mpris::signalUpdate(const QString &property, const QVariant &value) {
    QVariantMap map;
    map.insert(property, value);
    signalUpdate(map);
}

void Mpris::signalUpdate(const QVariantMap &map) {
    if (map.isEmpty()) {
        return;
    }
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
                                                     "org.freedesktop.DBus.Properties",
                                                     "PropertiesChanged");
    QVariantList args = QVariantList()
                          << "org.mpris.MediaPlayer2.Player"
                          << map
                          << QStringList();
    signal.setArguments(args);
    QDBusConnection::sessionBus().send(signal);
}

QString Mpris::currentTrackId() const {
    return QString("/org/mpris/MediaPlayer2/Track/%1").arg(status.id);
}
