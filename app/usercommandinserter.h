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

#ifndef USERCOMMANDINSERTER_H
#define USERCOMMANDINSERTER_H

#include <QObject>
#include <QStringList>

class QMenu;
class QPlainTextEdit;

class UserCommandInserter : public QObject
{
	Q_OBJECT

public:
	UserCommandInserter(QWidget *parent = 0);

	void readSettings();
	void setEditor(QPlainTextEdit *textEdit);
	QMenu *getMenu();
	QStringList getCommandWords();

signals:
	void updateCompleter();

private slots:
	void insertTag();
	void editCommands();

private:
	void updateMenu();

	QWidget *m_parentWidget;
	QMenu *m_userMenu;
	QPlainTextEdit *m_mainTextEdit;
	QStringList m_names;
	QStringList m_commands;
};

#endif //USERCOMMANDINSERTER_H
