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

#include "editindentdialog.h"

IndentDialog::IndentDialog(QWidget *parent, const QString &name)
    : QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle(name);

	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(ui.radioButtonSpaces);
	buttonGroup->addButton(ui.radioButtonTabs);
	buttonGroup->setExclusive(true);

	ui.radioButtonTabs->setFocus();
}

IndentDialog::~IndentDialog()
{
}

int IndentDialog::numOfInserts() const
{
	if (ui.radioButtonSpaces->isChecked())
		return ui.spinBoxSpaces->value();
	return ui.spinBoxTabs->value();
}

QChar IndentDialog::insertChar() const
{
	if (ui.radioButtonSpaces->isChecked())
		return ' ';
	return '\t';
}
