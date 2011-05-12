/***************************************************************************
 *   Copyright (C) 2007, 2011 by Glad Deschrijver                          *
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

#include "editindentwidget.h"

#include <QKeyEvent>
#include <QSettings>

#include "../common/utils/icon.h"

IndentWidget::IndentWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.pushButtonClose->setIcon(Icon("dialog-cancel"));

	readSettings();

	connect(ui.pushButtonIndent, SIGNAL(clicked()), this, SLOT(indent()));
	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(hide()));
}

IndentWidget::~IndentWidget()
{
}

void IndentWidget::readSettings()
{
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup("Editor");
	ui.radioButtonSpaces->setChecked(settings.value("Indent/InsertChar", '\t').toChar() == ' ');
	ui.radioButtonTabs->setChecked(settings.value("Indent/InsertChar", '\t').toChar() != ' ');
	ui.spinBoxSpaces->setValue(settings.value("Indent/NumberOfSpaces", 2).toInt());
	ui.spinBoxTabs->setValue(settings.value("Indent/NumberOfTabs", 1).toInt());
	settings.endGroup();
}

void IndentWidget::writeSettings()
{
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup("Editor");
	settings.setValue("Indent/InsertChar", ui.radioButtonSpaces->isChecked() ? QVariant::fromValue(' ') : QVariant::fromValue('\t'));
	settings.setValue("Indent/NumberOfSpaces", ui.spinBoxSpaces->value());
	settings.setValue("Indent/NumberOfTabs", ui.spinBoxTabs->value());
	settings.endGroup();
}

void IndentWidget::setUnindenting(bool isUnindenting)
{
	m_isUnindenting = isUnindenting;
	setWindowTitle(m_isUnindenting ? tr("Unindent") : tr("Indent"));
	ui.pushButtonIndent->setText(m_isUnindenting ? tr("Unin&dent") : tr("In&dent"));
}

QChar IndentWidget::insertChar() const
{
	return ui.radioButtonSpaces->isChecked() ? ' ' : '\t';
}

int IndentWidget::numOfInserts() const
{
	return ui.radioButtonSpaces->isChecked() ? ui.spinBoxSpaces->value() : ui.spinBoxTabs->value();
}

void IndentWidget::indent()
{
	const QChar insertChar = ui.radioButtonSpaces->isChecked() ? ' ' : '\t';
	const int numOfInserts = ui.radioButtonSpaces->isChecked() ? ui.spinBoxSpaces->value() : ui.spinBoxTabs->value();
	emit indent(insertChar, numOfInserts, m_isUnindenting);
	writeSettings();
}

void IndentWidget::hide()
{
	setVisible(false);
	emit hidden();
	readSettings();
}

void IndentWidget::showEvent(QShowEvent *event)
{
	if (ui.radioButtonSpaces->isChecked())
		setFocusProxy(ui.radioButtonSpaces);
	else
		setFocusProxy(ui.radioButtonTabs);
	QWidget::showEvent(event);
}

void IndentWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
		hide();
	else if (event->key() == Qt::Key_Return)
		indent();
	QWidget::keyPressEvent(event);
}
