/*
 * Melodeon - LMS Controller
 *
 * Copyright (c) 2022-2023 Craig Drummond <craig.p.drummond@gmail.com>
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

#ifndef STATUS_H
#define STATUS_H

#include <QtCore/QString>

struct Status {
    enum Shuffle {
        ShuffleOff = 0,
        ShufflePlaylist = 1,
        ShuffleAlbum = 2
    };
    enum Repeat {
        RepeatOff = 0,
        RepeatSingle = 1,
        RepeatAll = 2
    };

    bool playing = false;
    uint count = 0;
    QString title;
    QString artist;
    QString album;
    double duration = 0.0;
    double time = 0.0;
    QString id;
    Shuffle shuffle;
    Repeat repeat;
    uint volume;
};

#endif
