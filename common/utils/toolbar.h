/***************************************************************************
 *   Copyright (C) 2011 by Glad Deschrijver                                *
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

#ifndef KTIKZ_TOOLBAR_H
#define KTIKZ_TOOLBAR_H

#ifdef KTIKZ_USE_KDE
#  include <KToolBar>

class ToolBar : public KToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(const QString &objectName, QWidget *parent);
};
#else
#  include <QtCore/QtGlobal>
#  include <QtWidgets/QToolBar>

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(const QString &objectName, QWidget *parent);

private:
    void setToolBarStyle();
};
#endif

#endif
