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

#include "settings.h"

Settings * Settings::self() {
    static Settings *inst = new Settings();
    return inst;
}

Settings::Settings() {
    modified = false;
}

Settings::~Settings() {
    save();
}

void Settings::setName(const QString &val) {
    if (getName()!=val) {
        setValue("name", val);
        modified = true;
    }
}

QString Settings::getName() const {
    return getString("name", "");
}

void Settings::setAddress(const QString &val) {
    if (getAddress()!=val) {
        setValue("address", val);
        modified = true;
    }
}

QString Settings::getAddress() const {
    return getString("address", "");
}

void Settings::setPort(const uint val) {
    if (getPort()!=val) {
        setValue("port", val);
        modified = true;
    }
}

uint Settings::getPort() const {
    return getUInt("port", 9000);
}

void Settings::setZoom(const qreal val) {
    if (getZoom()!=val) {
        setValue("zoom", val);
        modified = true;
    }
}

void Settings::setUsername(const QString &val) {
    if (getUsername()!=val) {
        setValue("username", val);
        modified = true;
    }
}

QString Settings::getUsername() const {
    return getString("username", "");
}

void Settings::setPassword(const QString &val) {
    if (getPassword()!=val) {
        setValue("password", val);
        modified = true;
    }
}

QString Settings::getPassword() const {
    return getString("password", "");
}

qreal Settings::getZoom() const {
    return getReal("zoom", 1.0);
}

void Settings::setGeometry(const QByteArray &val) {
    setValue("geometry", val);
}

QByteArray Settings::getGeometry() const {
    return getByteArray("geometry", QByteArray());
}

void Settings::setState(const QByteArray &val) {
    setValue("state", val);
}

QByteArray Settings::getState() const {
    return getByteArray("state", QByteArray());
}

void Settings::setDark(bool val) {
    if (getDark()!=val) {
        setValue("dark", val);
        modified = true;
    }
}

bool Settings::getDark() {
    return getBool("dark", false);
}

void Settings::setInhibitSuspend(bool val) {
    if (getInhibitSuspend()!=val) {
        setValue("inhibitSuspend", val);
        modified = true;
    }
}
bool Settings::getInhibitSuspend() {
    return getBool("inhibitSuspend", false);
}

void Settings::setCustomTitlebar(bool val) {
    if (getCustomTitlebar()!=val) {
        setValue("customTitlebar", val);
        modified = true;
    }
}

bool Settings::getCustomTitlebar() const {
    return getBool("customTitlebar", false);
}

QString Settings::getString(const QString &key, const QString &def) const {
    return contains(key) ? value(key).toString() : def;
}

uint Settings::getUInt(const QString &key, const uint def) const {
    return contains(key) ? value(key).toUInt() : def;
}

qreal Settings::getReal(const QString &key, const qreal def) const {
    return contains(key) ? value(key).toReal() : def;
}

bool Settings::getBool(const QString &key, const bool def) const {
    return contains(key) ? value(key).toBool() : def;
}

QByteArray Settings::getByteArray(const QString &key, const QByteArray &def) const {
    return contains(key) ? value(key).toByteArray() : def;
}

void Settings::save() {
    if (modified) {
        sync();
        modified = false;
    }
}
