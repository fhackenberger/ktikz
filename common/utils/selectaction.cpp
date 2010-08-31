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

#include "selectaction.h"

#include "action.h"
#include "icon.h"

#ifdef KTIKZ_USE_KDE
#include <KActionCollection>

SelectAction::SelectAction(QObject *parent, const QString &name)
    : KSelectAction(parent)
{
	if (!name.isEmpty())
		Action::actionCollection()->addAction(name, this);
}

SelectAction::SelectAction(const QString &text, QObject *parent, const QString &name)
    : KSelectAction(text, parent)
{
	if (!name.isEmpty())
		Action::actionCollection()->addAction(name, this);
}

SelectAction::SelectAction(const Icon &icon, const QString &text, QObject *parent, const QString &name)
    : KSelectAction(icon, text, parent)
{
	if (!name.isEmpty())
		Action::actionCollection()->addAction(name, this);
}
#else
#include <QComboBox>
#include <QLineEdit>
#include <QWidgetAction>

SelectAction::SelectAction(QObject *parent, const QString &name)
    : QWidgetAction(parent)
{
	init(name);
}

SelectAction::SelectAction(const QString &text, QObject *parent, const QString &name)
    : QWidgetAction(parent)
{
	init(name);
	setText(text);
}

SelectAction::SelectAction(const Icon &icon, const QString &text, QObject *parent, const QString &name)
    : QWidgetAction(parent)
{
	init(name);
	setIcon(icon);
	setText(text);
}

void SelectAction::init(const QString &name)
{
	if (!name.isEmpty())
		setObjectName(name);

	m_selectCombo = new QComboBox;
	setDefaultWidget(m_selectCombo);
	connect(m_selectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentItem()));
}

SelectAction::~SelectAction()
{
	delete m_selectCombo;
}

void SelectAction::setEditable(bool editable)
{
	m_selectCombo->setEditable(editable);
	if (editable)
		connect(m_selectCombo->lineEdit(), SIGNAL(returnPressed()), this, SLOT(setCurrentItem()));
}

void SelectAction::setCurrentItem()
{
	const QString text = m_selectCombo->lineEdit()->text();
	emit triggered(text);
}

void SelectAction::removeAllActions()
{
	m_selectCombo->clear();
}

void SelectAction::setItems(const QStringList &items)
{
	m_selectCombo->clear();
	m_selectCombo->addItems(items);
}

void SelectAction::setCurrentItem(int index)
{
	m_selectCombo->setCurrentIndex(index);
	m_selectCombo->lineEdit()->setText(m_selectCombo->currentText());
}

QStringList SelectAction::items() const
{
	QStringList itemList;
	for (int i = 0; i < m_selectCombo->count(); ++i)
		itemList << m_selectCombo->itemText(i);
	return itemList;
}
#endif
