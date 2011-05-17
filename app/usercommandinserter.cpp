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

#include "usercommandinserter.h"

#include <QMenu>
#include <QPlainTextEdit>
#include <QPointer>
#include <QSettings>
#include <QTextCursor>

#include "usercommandeditdialog.h"

static const QString s_completionPlaceHolder(0x2022);

UserCommandInserter::UserCommandInserter(QWidget *parent)
	: QObject(parent)
{
	m_parentWidget = parent;
	m_mainTextEdit = 0;
	readSettings();
}

void UserCommandInserter::readSettings()
{
	m_names.clear();
	m_commands.clear();
	QSettings settings(ORGNAME, APPNAME);
	const int size = settings.beginReadArray("UserCommands");
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		m_names.append(settings.value("Name").toString());
		m_commands.append(settings.value("Command").toString());
	}
	settings.endArray();

	if (m_userMenu)
		updateMenu();
}

void UserCommandInserter::setEditor(QPlainTextEdit *textEdit)
{
	m_mainTextEdit = textEdit;
}

QMenu *UserCommandInserter::getMenu()
{
	m_userMenu = new QMenu(tr("&User snippets"), m_parentWidget);
	updateMenu();
	return m_userMenu;
}

void UserCommandInserter::updateMenu()
{
	m_userMenu->clear();

	QAction *action;
	for (int i = 0; i < m_names.size(); ++i)
	{
		action = new QAction(m_names.at(i), m_userMenu);
		action->setData(i);
		connect(action, SIGNAL(triggered()), this, SLOT(insertTag()));
		m_userMenu->addAction(action);
	}

	m_userMenu->addSeparator();

	action = new QAction(tr("&Edit user commands"), m_userMenu);
	connect(action, SIGNAL(triggered()), this, SLOT(editCommands()));
	m_userMenu->addAction(action);
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
	Q_ASSERT_X(m_mainTextEdit, "insertTag", "define m_mainTextEdit using UserCommandInserter::setEditor()");

	QAction *action = qobject_cast<QAction*>(sender());
	if (!action)
		return;

	const QString command = m_commands.at(action->data().toInt());
	QTextCursor textCursor = m_mainTextEdit->textCursor();
	const int position = textCursor.position();

	m_mainTextEdit->insertPlainText(command);

	textCursor.setPosition(position, QTextCursor::MoveAnchor);
	if (command.contains(s_completionPlaceHolder))
	{
		textCursor = m_mainTextEdit->document()->find(s_completionPlaceHolder, textCursor);
		m_mainTextEdit->setTextCursor(textCursor);
	}

	m_mainTextEdit->setFocus();
}

void UserCommandInserter::editCommands()
{
	QPointer<UserCommandEditDialog> editDialog = new UserCommandEditDialog(m_parentWidget);
	if (editDialog->exec())
	{
		readSettings();
		emit updateCompleter();
	}
	delete editDialog;
}
