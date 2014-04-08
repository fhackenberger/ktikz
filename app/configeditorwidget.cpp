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
	if (key == QLatin1String("ShowLineNumberArea"))
		return true;
	else if (key == QLatin1String("Font"))
		return qApp->font().toString();
	else if (key == QLatin1String("ShowWhiteSpaces"))
		return false;
	else if (key == QLatin1String("ShowTabulators"))
		return false;
	else if (key == QLatin1String("ShowMatchingBrackets"))
		return true;
	else if (key == QLatin1String("ColorWhiteSpaces"))
		return QColor(Qt::gray);
	else if (key == QLatin1String("ColorTabulators"))
		return QColor(Qt::gray);
	else if (key == QLatin1String("ColorMatchingBrackets"))
		return QColor(Qt::darkGreen);
	else if (key == QLatin1String("ShowHighlightCurrentLine"))
		return true;
	else if (key == QLatin1String("ColorHighlightCurrentLine"))
	{
		const QColor lineColor(QApplication::palette().color(QPalette::Normal, QPalette::Base));
		return lineColor.darker(105);
	}
	else if (key == QLatin1String("UseCompletion"))
		return true;
	return QVariant();
}

void ConfigEditorWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	ui.showLineNumberAreaCheck->setChecked(settings.value(QLatin1String("ShowLineNumberArea"), defaultSetting(QLatin1String("ShowLineNumberArea"))).toBool());
	m_generalFont.fromString(settings.value(QLatin1String("Font"), defaultSetting(QLatin1String("Font"))).toString());
	ui.generalFontEdit->setText(m_generalFont.family() + QLatin1Char(' ') + QString::number(m_generalFont.pointSize()));
	ui.generalFontEdit->setFont(m_generalFont);
	ui.showWhiteSpacesCheck->setChecked(settings.value(QLatin1String("ShowWhiteSpaces"), defaultSetting(QLatin1String("ShowWhiteSpaces"))).toBool());
	ui.showTabulatorsCheck->setChecked(settings.value(QLatin1String("ShowTabulators"), defaultSetting(QLatin1String("ShowTabulators"))).toBool());
	ui.showMatchingBracketsCheck->setChecked(settings.value(QLatin1String("ShowMatchingBrackets"), defaultSetting(QLatin1String("ShowMatchingBrackets"))).toBool());
	ui.whiteSpacesColorButton->setColor(settings.value(QLatin1String("ColorWhiteSpaces"), defaultSetting(QLatin1String("ColorWhiteSpaces"))).value<QColor>());
	ui.tabulatorsColorButton->setColor(settings.value(QLatin1String("ColorTabulators"), defaultSetting(QLatin1String("ColorTabulators"))).value<QColor>());
	ui.matchingBracketsColorButton->setColor(settings.value(QLatin1String("ColorMatchingBrackets"), defaultSetting(QLatin1String("ColorMatchingBrackets"))).value<QColor>());
	ui.highlightCurrentLineCheck->setChecked(settings.value(QLatin1String("ShowHighlightCurrentLine"), defaultSetting(QLatin1String("ShowHighlightCurrentLine"))).toBool());
	ui.highlightCurrentLineColorButton->setColor(settings.value(QLatin1String("ColorHighlightCurrentLine"), defaultSetting(QLatin1String("ColorHighlightCurrentLine"))).value<QColor>());

	ui.useCompletionCheck->setChecked(settings.value(QLatin1String("UseCompletion"), defaultSetting(QLatin1String("UseCompletion"))).toBool());
	settings.endGroup();
}

void ConfigEditorWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	settings.setValue(QLatin1String("ShowLineNumberArea"), ui.showLineNumberAreaCheck->isChecked());
	settings.setValue(QLatin1String("Font"), m_generalFont.toString());
	settings.setValue(QLatin1String("ShowWhiteSpaces"), ui.showWhiteSpacesCheck->isChecked());
	settings.setValue(QLatin1String("ShowTabulators"), ui.showTabulatorsCheck->isChecked());
	settings.setValue(QLatin1String("ShowMatchingBrackets"), ui.showMatchingBracketsCheck->isChecked());
	settings.setValue(QLatin1String("ColorWhiteSpaces"), ui.whiteSpacesColorButton->color());
	settings.setValue(QLatin1String("ColorTabulators"), ui.tabulatorsColorButton->color());
	settings.setValue(QLatin1String("ColorMatchingBrackets"), ui.matchingBracketsColorButton->color());
	settings.setValue(QLatin1String("ShowHighlightCurrentLine"), ui.highlightCurrentLineCheck->isChecked());
	settings.setValue(QLatin1String("ColorHighlightCurrentLine"), ui.highlightCurrentLineColorButton->color());
	settings.setValue(QLatin1String("UseCompletion"), ui.useCompletionCheck->isChecked());
	settings.endGroup();
}

void ConfigEditorWidget::selectFont()
{
	bool ok;
	const QFont newFont = FontDialog::getFont(&ok, m_generalFont, this);
	if (ok)
	{
		m_generalFont = newFont;
		ui.generalFontEdit->setText(m_generalFont.family() + QLatin1Char(' ') + QString::number(m_generalFont.pointSize()));
		ui.generalFontEdit->setFont(m_generalFont);
	}
}
