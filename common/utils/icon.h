/***************************************************************************
 *   Copyright (C) 2009, 2012 by Glad Deschrijver                          *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef KTIKZ_ICON_H
#define KTIKZ_ICON_H

#include <QtGui/QIcon>

class Icon : public QIcon
{
public:
    // Check why second option might make more sense
    // #ifdef KTIKZ_USE_DESKTOP_ICONS
    explicit Icon(const QString &iconName)
        : QIcon(QIcon::fromTheme(
                iconName, QIcon(QLatin1String(":/icons/") + iconName + QLatin1String(".png"))))
    {
    }
    // #else
    //	explicit Icon(const QString &iconName) : QIcon(QLatin1String(":/icons/") + iconName +
    //QLatin1String(".png")) {} // faster than the above #endif
    explicit Icon(const QIcon &copy) : QIcon(copy) { }
    Icon() : QIcon() { }
};

#endif
