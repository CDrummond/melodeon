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

#ifndef MPRIS_H
#define MPRIS_H

#include "config.h"
#include "player.h"
#include "status.h"
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtDBus/QDBusObjectPath>
#include <QtWidgets/QApplication>

class Mpris : public QObject {
    Q_OBJECT

    // org.mpris.MediaPlayer2.Player
    Q_PROPERTY( double Rate READ Rate WRITE SetRate )
    Q_PROPERTY( qlonglong Position READ Position )
    Q_PROPERTY( double MinimumRate READ MinimumRate )
    Q_PROPERTY( double MaximumRate READ MaximumRate )
    Q_PROPERTY( bool CanControl READ CanControl )
    Q_PROPERTY( bool CanPlay READ CanPlay )
    Q_PROPERTY( bool CanPause READ CanPause )
    Q_PROPERTY( bool CanSeek READ CanSeek )
    Q_PROPERTY( bool CanGoNext READ CanGoNext )
    Q_PROPERTY( bool CanGoPrevious READ CanGoPrevious )
    Q_PROPERTY( QString PlaybackStatus READ PlaybackStatus )
    Q_PROPERTY( QString LoopStatus READ LoopStatus WRITE SetLoopStatus )
    Q_PROPERTY( bool Shuffle READ Shuffle WRITE SetShuffle )
    Q_PROPERTY( QVariantMap Metadata READ Metadata )
    Q_PROPERTY( double Volume READ Volume WRITE SetVolume )

    // org.mpris.MediaPlayer2
    Q_PROPERTY( bool CanQuit READ CanQuit )
    Q_PROPERTY( bool CanRaise READ CanRaise )
    Q_PROPERTY( QString DesktopEntry READ DesktopEntry )
    Q_PROPERTY( bool HasTrackList READ HasTrackList )
    Q_PROPERTY( QString Identity READ Identity )
    Q_PROPERTY( QStringList SupportedMimeTypes READ SupportedMimeTypes )
    Q_PROPERTY( QStringList SupportedUriSchemes READ SupportedUriSchemes )

public:
    Mpris(Player *p);

    // org.mpris.MediaPlayer2.Player
    void Next() { player->next(); }
    void Previous() { player->prev(); }
    void Pause() { player->pause(); }
    void PlayPause() { status.playing ? player->pause() : player->play(); }
    void Stop() { player->stop(); }
    void Play() { player->play(); }
    void Seek(qlonglong pos);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong pos);
    void OpenUri(const QString &) { }
    QString PlaybackStatus() const;
    QString LoopStatus();
    void SetLoopStatus(const QString &s);
    QVariantMap Metadata() const;
    int Rate() const { return 1.0; }
    void SetRate(double) { }
    bool Shuffle() { return Status::ShuffleOff!=status.shuffle; }
    void SetShuffle(bool s);
    double Volume() const { return status.volume/100.0; }
    void SetVolume(double v) { player->setVolume(v*100); }
    qlonglong Position();
    double MinimumRate() const { return 1.0; }
    double MaximumRate() const { return 1.0; }
    bool CanControl() const { return true; }
    bool CanPlay() const { return status.count>0 && !status.playing; }
    bool CanPause() const { return status.count>0 && status.playing; }
    bool CanSeek() const { return status.count>0 && status.duration>=0; }
    bool CanGoNext() const { return status.count>0; }
    bool CanGoPrevious() const { return status.count>0; }

    // org.mpris.MediaPlayer2.root
    bool CanQuit() const { return true; }
    bool CanRaise() const { return false; } // TODO???
    bool HasTrackList() const { return false; }
    QString Identity() const { return QLatin1String("LMS"); }
    QString DesktopEntry() const { return QLatin1String(PROJECT_NAME); }
    QStringList SupportedUriSchemes() const { return QStringList(); }
    QStringList SupportedMimeTypes() const { return QStringList(); }

public slots:
    void Raise() { } // TODO???
    void Quit() { QApplication::quit(); }
    void statusUpdate(const Status &status);
    void setCover(const QString &id, const QString &url);

private:
    void signalUpdate(const QString &property, const QVariant &value);
    void signalUpdate(const QVariantMap &map);
    QString currentTrackId() const;

private:
    Player *player;
    Status status;
    QString coverId;
    QString coverUrl;
};

#endif
