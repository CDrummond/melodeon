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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QByteArray>
#include <QtCore/QSettings>

class Settings : public QSettings {
    Settings();
    virtual ~Settings();

public:
    static Settings *self();

    void setName(const QString &val);
    QString getName() const;
    void setAddress(const QString &val);
    QString getAddress() const;
    void setPort(const uint val);
    uint getPort() const;
    void setZoom(const qreal val);
    qreal getZoom() const;
    void setGeometry(const QByteArray &val);
    QByteArray getGeometry() const;
    void setState(const QByteArray &val);
    QByteArray getState() const;
    void setDark(bool val);
    bool getDark();

    void save();

private:
    QString getString(const QString &key, const QString &def) const;
    uint getUInt(const QString &key, const uint def) const;
    qreal getReal(const QString &key, const qreal def) const;
    bool getBool(const QString &key, const bool def) const;
    QByteArray getByteArray(const QString &key, const QByteArray &def) const;

private:
    bool modified;
};

#endif
