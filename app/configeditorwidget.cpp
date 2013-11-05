/***************************************************************************
 *   Copyright (C) 2008, 2009, 2012, 2013 by Glad Deschrijver              *
 *     <glad.deschrijver@gmail.com>                                        *
 *   Copyright (C) 2013 by João Carreira <jfmcarreira@gmail.com>           *
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

#include "configeditorwidget.h"

#include <QtCore/QSettings>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "../common/utils/fontdialog.h"

ConfigEditorWidget::ConfigEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.generalFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
}

QVariant ConfigEditorWidget::defaultSetting(const QString &key)
{
	if (key == "ShowLineNumberArea")
		return true;
	else if (key == "Font")
		return qApp->font().toString();
	else if (key == "ShowWhiteSpaces")
		return false;
	else if (key == "ShowTabulators")
		return false;
	else if (key == "ShowMatchingBrackets")
		return true;
	else if (key == "ColorWhiteSpaces")
		return QColor(Qt::gray);
	else if (key == "ColorTabulators")
		return QColor(Qt::gray);
	else if (key == "ColorMatchingBrackets")
		return QColor(Qt::darkGreen);
	else if (key == "ShowHighlightCurrentLine")
		return true;
	else if (key == "ColorHighlightCurrentLine")
	{
		const QColor lineColor(QApplication::palette().color(QPalette::Normal, QPalette::Base));
		return lineColor.darker(105);
	}
	else if (key == "UseCompletion")
		return true;
	return QVariant();
}

void ConfigEditorWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	ui.showLineNumberAreaCheck->setChecked(settings.value("ShowLineNumberArea", defaultSetting("ShowLineNumberArea")).toBool());
	m_generalFont.fromString(settings.value("Font", defaultSetting("Font")).toString());
	ui.generalFontEdit->setText(m_generalFont.family() + ' ' + QString::number(m_generalFont.pointSize()));
	ui.generalFontEdit->setFont(m_generalFont);
	ui.showWhiteSpacesCheck->setChecked(settings.value("ShowWhiteSpaces", defaultSetting("ShowWhiteSpaces")).toBool());
	ui.showTabulatorsCheck->setChecked(settings.value("ShowTabulators", defaultSetting("ShowTabulators")).toBool());
	ui.showMatchingBracketsCheck->setChecked(settings.value("ShowMatchingBrackets", defaultSetting("ShowMatchingBrackets")).toBool());
	ui.whiteSpacesColorButton->setColor(settings.value("ColorWhiteSpaces", defaultSetting("ColorWhiteSpaces")).value<QColor>());
	ui.tabulatorsColorButton->setColor(settings.value("ColorTabulators", defaultSetting("ColorTabulators")).value<QColor>());
	ui.matchingBracketsColorButton->setColor(settings.value("ColorMatchingBrackets", defaultSetting("ColorMatchingBrackets")).value<QColor>());
	ui.highlightCurrentLineCheck->setChecked(settings.value("ShowHighlightCurrentLine", defaultSetting("ShowHighlightCurrentLine")).toBool());
	ui.highlightCurrentLineColorButton->setColor(settings.value("ColorHighlightCurrentLine", defaultSetting("ColorHighlightCurrentLine")).value<QColor>());

	ui.useCompletionCheck->setChecked(settings.value("UseCompletion", defaultSetting("UseCompletion")).toBool());
	settings.endGroup();
}

void ConfigEditorWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	settings.setValue("ShowLineNumberArea", ui.showLineNumberAreaCheck->isChecked());
	settings.setValue("Font", m_generalFont.toString());
	settings.setValue("ShowWhiteSpaces", ui.showWhiteSpacesCheck->isChecked());
	settings.setValue("ShowTabulators", ui.showTabulatorsCheck->isChecked());
	settings.setValue("ShowMatchingBrackets", ui.showMatchingBracketsCheck->isChecked());
	settings.setValue("ColorWhiteSpaces", ui.whiteSpacesColorButton->color());
	settings.setValue("ColorTabulators", ui.tabulatorsColorButton->color());
	settings.setValue("ColorMatchingBrackets", ui.matchingBracketsColorButton->color());
	settings.setValue("ShowHighlightCurrentLine", ui.highlightCurrentLineCheck->isChecked());
	settings.setValue("ColorHighlightCurrentLine", ui.highlightCurrentLineColorButton->color());
	settings.setValue("UseCompletion", ui.useCompletionCheck->isChecked());
	settings.endGroup();
}

void ConfigEditorWidget::selectFont()
{
	bool ok;
	const QFont newFont = FontDialog::getFont(&ok, m_generalFont, this);
	if (ok)
	{
		m_generalFont = newFont;
		ui.generalFontEdit->setText(m_generalFont.family() + ' ' + QString::number(m_generalFont.pointSize()));
		ui.generalFontEdit->setFont(m_generalFont);
	}
}
