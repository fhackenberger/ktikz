/***************************************************************************
 *   Copyright (C) 2011, 2012, 2014 by Glad Deschrijver                    *
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

#include "toolbar.h"

#ifdef KTIKZ_USE_KDE
ToolBar::ToolBar(const QString &objectName, QWidget *parent) : KToolBar(objectName, parent) { }
#else
#  include <QtCore/QSettings>

ToolBar::ToolBar(const QString &objectName, QWidget *parent) : QToolBar(parent)
{
    setObjectName(objectName);
    setToolBarStyle();
}

void ToolBar::setToolBarStyle()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));

    const int toolBarStyleNumber = settings.value(QLatin1String("ToolBarStyle"), 0).toInt();
    Qt::ToolButtonStyle toolBarStyle = Qt::ToolButtonIconOnly;
    switch (toolBarStyleNumber) {
    case 0:
        toolBarStyle = Qt::ToolButtonIconOnly;
        break;
    case 1:
        toolBarStyle = Qt::ToolButtonTextOnly;
        break;
    case 2:
        toolBarStyle = Qt::ToolButtonTextBesideIcon;
        break;
    case 3:
        toolBarStyle = Qt::ToolButtonTextUnderIcon;
        break;
    }
    setToolButtonStyle(toolBarStyle);
}
#endif
