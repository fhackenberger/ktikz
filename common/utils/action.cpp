/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#include "action.h"
#include "icon.h"

#ifdef KTIKZ_USE_KDE
#include <KActionCollection>

KActionCollection *Action::m_actionCollection = 0;

Action::Action(QObject *parent, const QString &name)
    : KAction(parent)
{
	if (m_actionCollection && !name.isEmpty())
		m_actionCollection->addAction(name, this);
}

Action::Action(const QString &text, QObject *parent, const QString &name)
    : KAction(text, parent)
{
	if (m_actionCollection && !name.isEmpty())
		m_actionCollection->addAction(name, this);
}

Action::Action(const Icon &icon, const QString &text, QObject *parent, const QString &name)
    : KAction(icon, text, parent)
{
	if (m_actionCollection && !name.isEmpty())
		m_actionCollection->addAction(name, this);
}

KActionCollection *Action::actionCollection()
{
	return m_actionCollection;
}

void Action::setActionCollection(KActionCollection *actionCollection)
{
	m_actionCollection = actionCollection;
}
#else
Action::Action(QObject *parent, const QString &name)
    : QAction(parent)
{
	if (!name.isEmpty())
		setObjectName(name);
}

Action::Action(const QString &text, QObject *parent, const QString &name)
    : QAction(text, parent)
{
	if (!name.isEmpty())
		setObjectName(name);
}

Action::Action(const Icon &icon, const QString &text, QObject *parent, const QString &name)
    : QAction(icon, text, parent)
{
	if (!name.isEmpty())
		setObjectName(name);
}
#endif
