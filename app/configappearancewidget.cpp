/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009 by Glad Deschrijver                    *
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

#include "configappearancewidget.h"

#include <QApplication>
#include <QButtonGroup>
#include <QSettings>
#include <QTextCharFormat>

#include "../common/utils/colordialog.h"
#include "../common/utils/fontdialog.h"

ConfigAppearanceWidget::ConfigAppearanceWidget(QWidget *parent)
    : QWidget(parent)
{
	m_itemMargin = 10;

	ui.setupUi(this);

	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(ui.standardAppearanceCheck);
	buttonGroup->addButton(ui.customAppearanceCheck);
	buttonGroup->setExclusive(true);
	connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleCustom()));

	QPalette palette = ui.itemTable->palette();
	QColor highlightBackgroundColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::AlternateBase));
	if (highlightBackgroundColor == QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Base))
		highlightBackgroundColor = highlightBackgroundColor.darker(110);
	palette.setColor(QPalette::Normal, QPalette::Highlight, highlightBackgroundColor);
	palette.setColor(QPalette::Normal, QPalette::HighlightedText, QPalette::Text);
	palette.setColor(QPalette::Inactive, QPalette::Highlight, highlightBackgroundColor);
	palette.setColor(QPalette::Inactive, QPalette::HighlightedText, QPalette::Text);
	palette.setColor(QPalette::Disabled, QPalette::Highlight, highlightBackgroundColor);
	ui.itemTable->setPalette(palette);
	m_itemHighlighted = -1;
	connect(ui.itemTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)), this, SLOT(setItemHighlighted(QTableWidgetItem*)));

	connect(ui.fontButton, SIGNAL(clicked()), this, SLOT(showFontDialog()));
	connect(ui.colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
}

void ConfigAppearanceWidget::readSettings(const QString &settingsGroup)
{
	int num;
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup(settingsGroup);
	m_custom = settings.value("Customize", true).toBool();
	const int numOfRules = settings.value("Number", 0).toInt();
	for (int i = 0; i < numOfRules; ++i)
	{
		num = m_typeNames.indexOf(settings.value("Item" + QString::number(i) + "/Name").toString());
		if (num >= 0)
		{
			m_itemColors[num] = settings.value("Item" + QString::number(i) + "/Color").toString();
			m_itemFonts[num] = settings.value("Item" + QString::number(i) + "/Font").toString();
		}
	}
	settings.endGroup();

	setCustomizationType(m_custom);
	setItemFonts(m_itemFonts);
	setItemColors(m_itemColors);
}

void ConfigAppearanceWidget::writeSettings(const QString &settingsGroup)
{
	m_custom = getCustomizationType();

	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup(settingsGroup);
	settings.setValue("Customize", m_custom);
	if (m_custom)
	{
		for (int i = 0; i < m_typeNames.size(); ++i)
		{
			settings.setValue("Item" + QString::number(i) + "/Name", m_typeNames.at(i));
			settings.setValue("Item" + QString::number(i) + "/Color", m_itemColors.at(i));
			settings.setValue("Item" + QString::number(i) + "/Font", m_itemFonts.at(i));
		}
		settings.setValue("Number", m_typeNames.size());
	}
	settings.endGroup();
}

void ConfigAppearanceWidget::setItemHighlighted(QTableWidgetItem *item)
{
	m_itemHighlighted = ui.itemTable->row(item);
	setHighlightedForeground(m_itemColors.at(m_itemHighlighted));
	ui.fontButton->setEnabled(true);
	ui.colorButton->setEnabled(true);
}

void ConfigAppearanceWidget::setItemToolTip(QTableWidgetItem *item, const QFont &font)
{
	const QFontMetrics metrics(font);
	ui.itemTable->setRowHeight(m_itemHighlighted, metrics.height() + m_itemMargin);
	if (metrics.width(item->text()) >= ui.itemTable->contentsRect().width() - 30)
		item->setToolTip(item->text());
	else
		item->setToolTip("");
}

void ConfigAppearanceWidget::showEvent(QShowEvent*)
{
	for (int i = 0; i < ui.itemTable->rowCount(); ++i)
	{
		QFont font;
		font.fromString(m_itemFonts.at(i));
		setItemToolTip(ui.itemTable->item(i, 0), font);
	}

	if (m_itemHighlighted < 0) return;
	setHighlightedForeground(m_itemColors.at(m_itemHighlighted));
}

void ConfigAppearanceWidget::addItem(const QString &titleName)
{
	const int itemTableRowNum = ui.itemTable->rowCount();
	ui.itemTable->setRowCount(itemTableRowNum + 1);

	QTableWidgetItem *item = new QTableWidgetItem;
	item->setText(titleName);
	item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	ui.itemTable->setItem(itemTableRowNum, 0, item);
}

void ConfigAppearanceWidget::addItemFont(const QString &fontName)
{
	const int rowNum = m_itemFonts.size();
	m_itemFonts.append(fontName);

	QFont font;
	font.fromString(fontName);
	ui.itemTable->item(rowNum, 0)->setFont(font);
	ui.itemTable->setRowHeight(rowNum, QFontMetrics(font).height() + m_itemMargin);
}

void ConfigAppearanceWidget::addItemColor(const QString &colorName)
{
	const int rowNum = m_itemFonts.size();
	m_itemColors.append(colorName);
	ui.itemTable->item(rowNum, 0)->setForeground(QColor(colorName));
	toggleCustom();
}

void ConfigAppearanceWidget::addItems(const QStringList &titleNames)
{
	for (int i = 0; i < titleNames.size(); ++i)
		addItem(titleNames.at(i));
}

void ConfigAppearanceWidget::setItemFonts(const QStringList &fontNames)
{
	m_itemFonts = fontNames;
	for (int i = 0; i < m_itemFonts.size(); ++i)
	{
		QFont font;
		font.fromString(m_itemFonts.at(i));
		ui.itemTable->item(i, 0)->setFont(font);
		ui.itemTable->setRowHeight(i, QFontMetrics(font).height() + m_itemMargin);
	}
}

void ConfigAppearanceWidget::setItemColors(const QStringList &colorNames)
{
	m_itemColors = colorNames;
	for (int i = 0; i < m_itemColors.size(); ++i)
		ui.itemTable->item(i, 0)->setForeground(QColor(m_itemColors.at(i)));
	toggleCustom();
}

QStringList ConfigAppearanceWidget::getItemFonts()
{
	return m_itemFonts;
}

QStringList ConfigAppearanceWidget::getItemColors()
{
	return m_itemColors;
}

void ConfigAppearanceWidget::setCustomizationType(bool custom)
{
	if (custom)
		ui.customAppearanceCheck->setChecked(true);
	else
		ui.standardAppearanceCheck->setChecked(true);
	toggleCustom();
}

bool ConfigAppearanceWidget::getCustomizationType()
{
	return !ui.standardAppearanceCheck->isChecked();
}

void ConfigAppearanceWidget::toggleCustom()
{
	if (ui.customAppearanceCheck->isChecked())
	{
		ui.appearanceGroupBox->setEnabled(true);
		for (int i = 0; i < m_itemColors.size(); ++i)
			ui.itemTable->item(i, 0)->setForeground(QColor(m_itemColors.at(i)));
	}
	else
	{
		ui.appearanceGroupBox->setEnabled(false);
		for (int i = 0; i < m_itemColors.size(); ++i)
			ui.itemTable->item(i, 0)->setForeground(QApplication::style()->standardPalette().color(QPalette::Disabled, QPalette::Text));
	}
}

void ConfigAppearanceWidget::setTypeNames(const QStringList &typeNames)
{
	m_typeNames = typeNames;
}

void ConfigAppearanceWidget::setDefaultTextCharFormats(const QMap<QString, QTextCharFormat> &defaultFormatList)
{
	for (int i = 0; i < defaultFormatList.size(); ++i)
	{
		m_itemColors << "";
		m_itemFonts << "";
	}

	QMap<QString, QTextCharFormat>::const_iterator it = defaultFormatList.constBegin();
	int num;
	while (it != defaultFormatList.constEnd())
	{
		num = m_typeNames.indexOf(it.key());
		if (num < 0) continue;
		m_itemColors[num] = it.value().foreground().color().name();
		m_itemFonts[num] = it.value().font().toString();
		++it;
	}
}

void ConfigAppearanceWidget::showFontDialog()
{
	if (m_itemHighlighted < 0) return;

	bool ok;
	QFont currentFont;
	currentFont.fromString(m_itemFonts.at(m_itemHighlighted));
	const QFont newFont = FontDialog::getFont(&ok, currentFont, this);
	if (ok)
	{
		QTableWidgetItem *item = ui.itemTable->item(m_itemHighlighted, 0);
		m_itemFonts.replace(m_itemHighlighted, newFont.toString());
		item->setFont(newFont);
		setItemToolTip(item, newFont);
	}
}

void ConfigAppearanceWidget::showColorDialog()
{
	if (m_itemHighlighted < 0) return;

	bool ok;
	const QColor currentColor(m_itemColors.at(m_itemHighlighted));
	const QColor newColor = ColorDialog::getColor(&ok, currentColor, this);
	if (ok)
	{
		m_itemColors.replace(m_itemHighlighted, newColor.name());
		ui.itemTable->item(m_itemHighlighted, 0)->setForeground(newColor);
		setHighlightedForeground(m_itemColors.at(m_itemHighlighted));
	}
}

void ConfigAppearanceWidget::setHighlightedForeground(const QString &colorName)
{
	QPalette palette = ui.itemTable->palette();
	const QColor color(colorName);
	palette.setColor(QPalette::Normal, QPalette::HighlightedText, color);
	palette.setColor(QPalette::Inactive, QPalette::HighlightedText, color);
	palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QApplication::style()->standardPalette().color(QPalette::Disabled, QPalette::Text));
	ui.itemTable->setPalette(palette);
}
