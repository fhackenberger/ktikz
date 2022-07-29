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

#include "usercommandinserter.h"

#include <QtCore/QPointer>
#include <QtCore/QSettings>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QMenu>
#else
#include <QtGui/QMenu>
#endif

#include "tikzcommandinserter.h"
#include "usercommandeditdialog.h"

static const QString s_completionPlaceHolder(QChar(0x2022));

UserCommandInserter::UserCommandInserter(QWidget *parent)
	: QObject(parent)
	, m_userMenu(0)
{
	readSettings();
}

void UserCommandInserter::readSettings()
{
	m_names.clear();
	m_commands.clear();
	QSettings settings;
	const int size = settings.beginReadArray(QLatin1String("UserCommands"));
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		m_names.append(settings.value(QLatin1String("Name")).toString());
		m_commands.append(settings.value(QLatin1String("Command")).toString());
	}
	settings.endArray();

	if (m_userMenu)
		updateMenu();
}

QMenu *UserCommandInserter::getMenu()
{
	if (!m_userMenu)
		m_userMenu = new QMenu(tr("&User snippets"), qobject_cast<QWidget*>(parent()));
	updateMenu();
	return m_userMenu;
}

void UserCommandInserter::updateMenu()
{
	m_userMenu->clear();

	for (int i = 0; i < m_names.size(); ++i)
	{
		QAction *action = m_userMenu->addAction(m_names.at(i));
		action->setData(i);
		connect(action, SIGNAL(triggered()), this, SLOT(insertTag()));
	}

	m_userMenu->addSeparator();

	QAction *action = m_userMenu->addAction(tr("&Edit user commands"));
	connect(action, SIGNAL(triggered()), this, SLOT(editCommands()));
}

/*
 * see TikzCommandInserter::getCommandWords()
 */

QStringList UserCommandInserter::getCommandWords()
{
	return m_commands;
}

/*
 * see TikzCommandInserter::insertTag(const QString &tag, int dx, int dy)
 */

void UserCommandInserter::insertTag()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (!action)
		return;
	Q_EMIT insertTag(m_commands.at(action->data().toInt()));
}

void UserCommandInserter::editCommands()
{
	QPointer<UserCommandEditDialog> editDialog = new UserCommandEditDialog(qobject_cast<QWidget*>(parent()));
	if (editDialog->exec())
	{
		readSettings();
		Q_EMIT updateCompleter();
	}
	delete editDialog;
}
