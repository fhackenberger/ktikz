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

#ifndef USERCOMMANDEDITDIALOG_H
#define USERCOMMANDEDITDIALOG_H

#include "ui_usercommandeditdialog.h"

class QMenu;

class UserCommandEditDialog : public QDialog
{
	Q_OBJECT

public:
	UserCommandEditDialog(QWidget *parent = 0);

	Ui::UserCommandEditDialog ui;

public slots:
	void accept();

private slots:
	void readSettings();
	void writeSettings();
	void setEditingEnabled(bool enabled);
	void addItem();
	void removeItem();
	void changeItem(int index);
	void insertPlaceHolder();

private:
	QStringList m_names;
	QStringList m_commands;
	int m_oldIndex;
};

#endif //USERCOMMANDEDITDIALOG_H
