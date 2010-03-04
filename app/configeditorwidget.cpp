/***************************************************************************
 *   Copyright (C) 2008-2009 by Glad Deschrijver                           *
 *   glad.deschrijver@gmail.com                                            *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "configeditorwidget.h"

#include <QFontDialog>
#include <QSettings>

ConfigEditorWidget::ConfigEditorWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.generalFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
}

void ConfigEditorWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	m_generalFont.fromString(settings.value("Font", qApp->font().toString()).toString());
	ui.generalFontEdit->setText(m_generalFont.family() + " " + QString::number(m_generalFont.pointSize()));
	ui.generalFontEdit->setFont(m_generalFont);
	ui.showWhiteSpacesCheck->setChecked(settings.value("ShowWhiteSpaces", false).toBool());
	ui.showTabulatorsCheck->setChecked(settings.value("ShowTabulators", false).toBool());
	ui.showMatchingBracketsCheck->setChecked(settings.value("ShowMatchingBrackets", true).toBool());
	ui.whiteSpacesColorButton->setColor(settings.value("ColorWhiteSpaces", Qt::gray).value<QColor>());
	ui.tabulatorsColorButton->setColor(settings.value("ColorTabulators", Qt::gray).value<QColor>());
	ui.matchingBracketsColorButton->setColor(settings.value("ColorMatchingBrackets", Qt::yellow).value<QColor>());
	ui.useCompletionCheck->setChecked(settings.value("UseCompletion", true).toBool());
	settings.endGroup();
}

void ConfigEditorWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	settings.setValue("Font", m_generalFont.toString());
	settings.setValue("ShowWhiteSpaces", ui.showWhiteSpacesCheck->isChecked());
	settings.setValue("ShowTabulators", ui.showTabulatorsCheck->isChecked());
	settings.setValue("ShowMatchingBrackets", ui.showMatchingBracketsCheck->isChecked());
	settings.setValue("ColorWhiteSpaces", ui.whiteSpacesColorButton->color());
	settings.setValue("ColorTabulators", ui.tabulatorsColorButton->color());
	settings.setValue("ColorMatchingBrackets", ui.matchingBracketsColorButton->color());
	settings.setValue("UseCompletion", ui.useCompletionCheck->isChecked());
	settings.endGroup();
}

void ConfigEditorWidget::selectFont()
{
	bool ok;
	const QFont newFont = QFontDialog::getFont(&ok, m_generalFont, this);
	if (ok)
	{
		m_generalFont = newFont;
		ui.generalFontEdit->setText(m_generalFont.family() + " " + QString::number(m_generalFont.pointSize()));
		ui.generalFontEdit->setFont(m_generalFont);
	}
}
