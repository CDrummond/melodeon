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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QByteArray>
#include <QtCore/QSettings>
#include <QtCore/QSize>

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
    void setUsername(const QString &val);
    QString getUsername() const;
    void setPassword(const QString &val);
    QString getPassword() const;
    void setZoom(const qreal val);
    qreal getZoom() const;
    void setWindowSize(const QSize &val);
    QSize getWindowSize() const;
    void setMaximized(bool val);
    bool getMaximized() const;

    void setDark(bool val);
    bool getDark() const;
    void setInhibitSuspend(bool val);
    bool getInhibitSuspend() const;
    void setCustomTitlebar(bool val);
    bool getCustomTitlebar() const;

    void save();

private:
    QString getString(const QString &key, const QString &def) const;
    uint getUInt(const QString &key, const uint def) const;
    qreal getReal(const QString &key, const qreal def) const;
    bool getBool(const QString &key, const bool def) const;
    QSize getSize(const QString &key, const QSize &def) const;

private:
    bool modified;
};

#endif
