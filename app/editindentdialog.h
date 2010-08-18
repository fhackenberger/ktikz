/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
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

#ifndef INDENTDIALOG_H
#define INDENTDIALOG_H

#include "ui_editindentdialog.h"

class IndentDialog : public QDialog
{
	Q_OBJECT

public:
	explicit IndentDialog(QWidget *parent = 0, const QString &name = 0);
	~IndentDialog();
	int numOfInserts() const;
	QChar insertChar() const;

private:
	Ui::IndentDialog ui;
};

#endif
