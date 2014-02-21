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

#ifndef KTIKZ_TOGGLEACTION_H
#define KTIKZ_TOGGLEACTION_H

class Icon;

#ifdef KTIKZ_USE_KDE
#include <KToggleAction>

class ToggleAction : public KToggleAction
{
	Q_OBJECT

public:
	explicit ToggleAction(QObject *parent, const QString &name = QString());
	ToggleAction(const QString &text, QObject *parent, const QString &name = QString());
	ToggleAction(const Icon &icon, const QString &text, QObject *parent, const QString &name = QString());
};
#else
#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QAction>
#else
#include <QtGui/QAction>
#endif

class ToggleAction : public QAction
{
	Q_OBJECT

public:
	explicit ToggleAction(QObject *parent, const QString &name = QString());
	ToggleAction(const QString &text, QObject *parent, const QString &name = QString());
	ToggleAction(const Icon &icon, const QString &text, QObject *parent, const QString &name = QString());

private:
	void init(const QString &name);
};
#endif

#endif
