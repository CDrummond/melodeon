/*
 * Melodeon - LMS Controller
 *
 * Copyright (c) 2022-2024 Craig Drummond <craig.p.drummond@gmail.com>
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

#include "svgicon.h"
#include <QtCore/QFile>
#include <QtCore/QRect>
#include <QtGui/QIconEngine>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>
#include <QtSvg/QSvgRenderer>

class SvgIconEngine : public QIconEngine {
public:
    SvgIconEngine(const QString &file, const QColor &col, const QColor &sel)
        : fileName(file)
        , color(col)
        , selectedColor(sel) {
    }

    ~SvgIconEngine() override {}

    SvgIconEngine * clone() const override {
        return new SvgIconEngine(fileName, color, selectedColor);
    }

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override {
        Q_UNUSED(state)

        QColor col=QIcon::Selected==mode ? selectedColor : color;
        QString key=fileName+QLatin1Char('-')+QString::number(rect.width())+QLatin1Char('-')+QString::number(rect.height())+QLatin1Char('-')+col.name();
        QPixmap pix;

        if (!QPixmapCache::find(key, &pix)) {
            pix=QPixmap(rect.width(), rect.height());
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            QSvgRenderer renderer;
            QFile f(fileName);
            QByteArray bytes;
            if (f.open(QIODevice::ReadOnly)) {
                bytes=f.readAll();
            }
            if (!bytes.isEmpty()) {
                bytes.replace("#000", col.name().toLatin1());
            }
            renderer.load(bytes);
            renderer.render(&p, QRect(0, 0, rect.width(), rect.height()));
            QPixmapCache::insert(key, pix);
        }
        if (QIcon::Disabled==mode) {
            painter->save();
            painter->setOpacity(painter->opacity()*0.35);
        }
        painter->drawPixmap(rect.topLeft(), pix);
        if (QIcon::Disabled==mode) {
            painter->restore();
        }
    }

    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override {
        QPixmap pix(size);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        paint(&painter, QRect(QPoint(0, 0), size), mode, state);
        return pix;
    }

private:
    QString fileName;
    QColor color;
    QColor selectedColor;
};

QIcon SvgIcon::icon(const QString &fileName, const QColor &col, const QColor &sel) {
    return QIcon(new SvgIconEngine(fileName, col, sel));
}