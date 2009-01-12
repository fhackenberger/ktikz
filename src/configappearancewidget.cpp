/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
 *   Glad.Deschrijver@UGent.be                                             *
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

#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QFontDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QTableWidget>
#include <QTextCharFormat>
#include <QVBoxLayout>

#include "configappearancewidget.h"

ConfigAppearanceWidget::ConfigAppearanceWidget(QWidget *parent)
    : QWidget(parent)
{
	m_itemMargin = 10;

	m_standardAppearanceCheck = new QRadioButton(tr("&Standard"));
	m_standardAppearanceCheck->setWhatsThis("<p>" + tr("When this option "
	    "is checked, the default fonts and colors are used in "
	    "the output.") + "</p>");
	m_customAppearanceCheck = new QRadioButton(tr("&Custom"));
	m_customAppearanceCheck->setWhatsThis("<p>" + tr("When this option "
	    "is checked, the fonts and colors defined below will be used "
	    "in the output instead of the default ones.") + "</p>");

	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(m_standardAppearanceCheck);
	buttonGroup->addButton(m_customAppearanceCheck);
	buttonGroup->setExclusive(true);
	connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleCustom()));

	m_itemTable = new QTableWidget(this);
	m_itemTable->setWhatsThis("<p>" + tr("Select the structure "
	    "appearing in the TikZ code for which you want to change "
	    "the fonts and colors.") + "</p>");
	m_itemTable->setShowGrid(true);
	m_itemTable->horizontalHeader()->hide();
	m_itemTable->verticalHeader()->hide();
	m_itemTable->setColumnCount(1);
	m_itemTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_itemTable->horizontalHeader()->setStretchLastSection(true);
	m_itemTable->setTabKeyNavigation(false);
	QPalette palette = m_itemTable->palette();
	QColor highlightBackgroundColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::AlternateBase));
	if (highlightBackgroundColor == QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Base))
		highlightBackgroundColor = highlightBackgroundColor.darker(110);
	palette.setColor(QPalette::Normal, QPalette::Highlight, highlightBackgroundColor);
	palette.setColor(QPalette::Normal, QPalette::HighlightedText, QPalette::Text);
	palette.setColor(QPalette::Inactive, QPalette::Highlight, highlightBackgroundColor);
	palette.setColor(QPalette::Inactive, QPalette::HighlightedText, QPalette::Text);
	palette.setColor(QPalette::Disabled, QPalette::Highlight, highlightBackgroundColor);
	m_itemTable->setPalette(palette);
	m_itemHighlighted = -1;
	connect(m_itemTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)), this, SLOT(setItemHighlighted(QTableWidgetItem*)));

	m_fontButton = new QPushButton(tr("Change fo&nt"));
	m_fontButton->setWhatsThis("<p>" + tr("Select the font in which "
	    "the structure that you selected in the box above should "
	    "be displayed.") + "</p>");
	m_fontButton->setEnabled(false);
	connect(m_fontButton, SIGNAL(clicked()), this, SLOT(showFontDialog()));

	m_colorButton = new QPushButton(tr("Change co&lor"));
	m_colorButton->setWhatsThis("<p>" + tr("Select the text color in "
	    "which the structure that you selected in the box above "
	    "should be displayed.") + "</p>");
	m_colorButton->setEnabled(false);
	connect(m_colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));

	m_appearanceWidget = new QGroupBox(tr("Change st&yle for") + ":");
	QGridLayout *appearanceLayout = new QGridLayout(m_appearanceWidget);
	appearanceLayout->addWidget(m_itemTable, 0, 0, 1, 3);
	appearanceLayout->addWidget(m_fontButton, 1, 1);
	appearanceLayout->addWidget(m_colorButton, 1, 2);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_standardAppearanceCheck);
	layout->addWidget(m_customAppearanceCheck);
	layout->addWidget(m_appearanceWidget);
	layout->setMargin(0);
}

void ConfigAppearanceWidget::setItemHighlighted(QTableWidgetItem *item)
{
	m_itemHighlighted = m_itemTable->row(item);
	setHighlightedForeground(m_itemColors.at(m_itemHighlighted));
	m_fontButton->setEnabled(true);
	m_colorButton->setEnabled(true);
}

void ConfigAppearanceWidget::setItemToolTip(QTableWidgetItem *item, const QFont &font)
{
	const QFontMetrics metrics(font);
	m_itemTable->setRowHeight(m_itemHighlighted, metrics.height() + m_itemMargin);
	if (metrics.width(item->text()) >= m_itemTable->contentsRect().width() - 30)
		item->setToolTip(item->text());
	else
		item->setToolTip("");
}

void ConfigAppearanceWidget::showEvent(QShowEvent*)
{
	for (int i = 0; i < m_itemTable->rowCount(); ++i)
	{
		QFont font;
		font.fromString(m_itemFonts.at(i));
		setItemToolTip(m_itemTable->item(i, 0), font);
	}

	if (m_itemHighlighted < 0) return;
	setHighlightedForeground(m_itemColors.at(m_itemHighlighted));
}

void ConfigAppearanceWidget::addItem(const QString &titleName)
{
	const int itemTableRowNum = m_itemTable->rowCount();
	m_itemTable->setRowCount(itemTableRowNum+1);

	QTableWidgetItem *item = new QTableWidgetItem;
	item->setText(titleName);
	item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	m_itemTable->setItem(itemTableRowNum, 0, item);
}

void ConfigAppearanceWidget::addItemFont(const QString &fontName)
{
	const int rowNum = m_itemFonts.size();
	m_itemFonts.append(fontName);

	QFont font;
	font.fromString(fontName);
	m_itemTable->item(rowNum, 0)->setFont(font);
	m_itemTable->setRowHeight(rowNum, QFontMetrics(font).height() + m_itemMargin);
}

void ConfigAppearanceWidget::addItemColor(const QString &colorName)
{
	int rowNum = m_itemFonts.size();
	m_itemColors.append(colorName);
	m_itemTable->item(rowNum, 0)->setForeground(QColor(colorName));
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
		m_itemTable->item(i, 0)->setFont(font);
		m_itemTable->setRowHeight(i, QFontMetrics(font).height() + m_itemMargin);
	}
}

void ConfigAppearanceWidget::setItemColors(const QStringList &colorNames)
{
	m_itemColors = colorNames;
	for (int i = 0; i < m_itemColors.size(); ++i)
		m_itemTable->item(i, 0)->setForeground(QColor(m_itemColors.at(i)));
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
		m_customAppearanceCheck->setChecked(true);
	else
		m_standardAppearanceCheck->setChecked(true);
	toggleCustom();
}

bool ConfigAppearanceWidget::getCustomizationType()
{
	return !m_standardAppearanceCheck->isChecked();
}

void ConfigAppearanceWidget::toggleCustom()
{
	if (m_customAppearanceCheck->isChecked())
	{
		m_appearanceWidget->setEnabled(true);
		for (int i = 0; i < m_itemColors.size(); ++i)
			m_itemTable->item(i, 0)->setForeground(QColor(m_itemColors.at(i)));
	}
	else
	{
		m_appearanceWidget->setEnabled(false);
		for (int i = 0; i < m_itemColors.size(); ++i)
			m_itemTable->item(i, 0)->setForeground(QApplication::style()->standardPalette().color(QPalette::Disabled, QPalette::Text));
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

void ConfigAppearanceWidget::readSettings(const QString &settingsGroup)
{
	int num;
	QSettings settings;
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
}

void ConfigAppearanceWidget::setSettings()
{
	setCustomizationType(m_custom);
	setItemFonts(m_itemFonts);
	setItemColors(m_itemColors);
}

void ConfigAppearanceWidget::writeSettings(const QString &settingsGroup)
{
	m_custom = getCustomizationType();

	QSettings settings;
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

void ConfigAppearanceWidget::showFontDialog()
{
	if (m_itemHighlighted < 0) return;

	bool ok;
	QFont currentFont;
	currentFont.fromString(m_itemFonts.at(m_itemHighlighted));
	const QFont newFont = QFontDialog::getFont(&ok, currentFont, this);
	if (ok)
	{
		QTableWidgetItem *item = m_itemTable->item(m_itemHighlighted, 0);
		m_itemFonts.replace(m_itemHighlighted, newFont.toString());
		item->setFont(newFont);
		setItemToolTip(item, newFont);
	}
}

void ConfigAppearanceWidget::showColorDialog()
{
	if (m_itemHighlighted < 0) return;

	const QColor currentColor(m_itemColors.at(m_itemHighlighted));
	const QColor newColor = QColorDialog::getColor(currentColor, this);
	if (newColor.isValid())
	{
		m_itemColors.replace(m_itemHighlighted, newColor.name());
		m_itemTable->item(m_itemHighlighted, 0)->setForeground(newColor);
		setHighlightedForeground(m_itemColors.at(m_itemHighlighted));
	}
}

void ConfigAppearanceWidget::setHighlightedForeground(const QString &colorName)
{
	QPalette palette = m_itemTable->palette();
	const QColor color(colorName);
	palette.setColor(QPalette::Normal, QPalette::HighlightedText, color);
	palette.setColor(QPalette::Inactive, QPalette::HighlightedText, color);
	palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QApplication::style()->standardPalette().color(QPalette::Disabled, QPalette::Text));
	m_itemTable->setPalette(palette);
}
