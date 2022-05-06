/***************************************************************************
 *   Copyright (C) 2011, 2012 by Glad Deschrijver                          *
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

#include "usercommandeditdialog.h"

#include <QtCore/QSettings>

static const QString s_completionPlaceHolder(QChar(0x2022));

UserCommandEditDialog::UserCommandEditDialog(QWidget *parent)
	: QDialog(parent),
	m_oldIndex(-1)
{
	setModal(true);
	ui.setupUi(this);

	ui.comboBoxItem->setMinimumContentsLength(QString(tr("Menu item 100")).length());

	connect(ui.pushButtonAdd, SIGNAL(clicked()), this, SLOT(addItem()));
	connect(ui.pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeItem()));
	connect(ui.comboBoxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(changeItem(int)));
	connect(ui.pushButtonInsertPlaceHolder, SIGNAL(clicked()), this, SLOT(insertPlaceHolder()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	readSettings();
}

void UserCommandEditDialog::readSettings()
{
	disconnect(ui.comboBoxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(changeItem(int)));
	QSettings settings;
	int size = settings.beginReadArray(QLatin1String("UserCommands"));
	ui.comboBoxItem->clear();
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		m_names.append(settings.value(QLatin1String("Name")).toString());
		m_commands.append(settings.value(QLatin1String("Command")).toString());

		ui.comboBoxItem->insertItem(i, QString(tr("Menu item %1").arg(QString::number(i+1))));
	}
	settings.endArray();
	connect(ui.comboBoxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(changeItem(int)));

	if (m_names.size() > 0)
	{
		setEditingEnabled(true);
		ui.comboBoxItem->setCurrentIndex(0);
		ui.lineEditName->setText(m_names.at(0));
		ui.textEditContent->setPlainText(m_commands.at(0));
		m_oldIndex = 0;
	}
	else
		setEditingEnabled(false);
}

void UserCommandEditDialog::writeSettings()
{
	if (m_oldIndex >= 0)
	{
		m_names[m_oldIndex] = ui.lineEditName->text();
		m_commands[m_oldIndex] = ui.textEditContent->toPlainText();
	}

	QSettings settings;
	settings.beginWriteArray(QLatin1String("UserCommands"));
	settings.remove(QString()); // remove old entries (especially useful when the number of entries decreases)
	for (int i = 0; i < m_names.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue(QLatin1String("Name"), m_names.at(i));
		settings.setValue(QLatin1String("Command"), m_commands.at(i));
	}
	settings.endArray();
}

void UserCommandEditDialog::accept()
{
	if (m_names.size() > 0 && m_oldIndex >= 0)
	{
		m_names[m_oldIndex] = ui.lineEditName->text();
		m_commands[m_oldIndex] = ui.textEditContent->toPlainText();
	}
	writeSettings();
	QDialog::accept();
}

void UserCommandEditDialog::setEditingEnabled(bool enabled)
{
	ui.labelItem->setEnabled(enabled);
	ui.comboBoxItem->setEnabled(enabled);
	ui.labelName->setEnabled(enabled);
	ui.lineEditName->setEnabled(enabled);
	ui.labelContent->setEnabled(enabled);
	ui.pushButtonInsertPlaceHolder->setEnabled(enabled);
	ui.textEditContent->setEnabled(enabled);
}

void UserCommandEditDialog::addItem()
{
	const int index = ui.comboBoxItem->count();
	m_names.append(QString());
	m_commands.append(QString());
	ui.comboBoxItem->addItem(QString(tr("Menu item %1").arg(QString::number(index+1))));
	ui.comboBoxItem->setCurrentIndex(index);
	setEditingEnabled(true);
}

void UserCommandEditDialog::removeItem()
{
	disconnect(ui.comboBoxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(changeItem(int)));

	int index = ui.comboBoxItem->currentIndex();
	m_names.removeAt(index);
	m_commands.removeAt(index);
	ui.comboBoxItem->removeItem(index);

	if (index >= m_names.size())
	{
		--index;
		--m_oldIndex;
	}
	ui.lineEditName->setText(index >= 0 ? m_names.at(index) : QString());
	ui.textEditContent->setPlainText(index >= 0 ? m_commands.at(index) : QString());

	if (index < 0)
		setEditingEnabled(false);

	connect(ui.comboBoxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(changeItem(int)));
}

void UserCommandEditDialog::changeItem(int index)
{
	if (m_oldIndex >= 0)
	{
		m_names[m_oldIndex] = ui.lineEditName->text();
		m_commands[m_oldIndex] = ui.textEditContent->toPlainText();
	}
	ui.lineEditName->setText(m_names.at(index));
	ui.textEditContent->setPlainText(m_commands.at(index));
	m_oldIndex = index;
}

void UserCommandEditDialog::insertPlaceHolder()
{
	ui.textEditContent->insertPlainText(s_completionPlaceHolder);
	ui.textEditContent->setFocus();
}
