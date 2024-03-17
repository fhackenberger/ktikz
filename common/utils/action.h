/***************************************************************************
 *   Copyright (C) 2009, 2014 by Glad Deschrijver                          *
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

#ifndef KTIKZ_ACTION_H
#define KTIKZ_ACTION_H

#include <QtGlobal>
#include <QAction>

class KActionCollection;
class Icon;

class Action : public QAction
{
    Q_OBJECT

public:
    explicit Action(QObject *parent, const QString &name = QString());
    Action(const QString &text, QObject *parent, const QString &name = QString());
    Action(const Icon &icon, const QString &text, QObject *parent, const QString &name = QString());

    void setShortcut(const QKeySequence &shortcut);

#ifdef KTIKZ_USE_KDE
    static KActionCollection *actionCollection();
    static void setActionCollection(KActionCollection *actionCollection);

private:
    static KActionCollection *s_actionCollection;
#endif
};

#endif
