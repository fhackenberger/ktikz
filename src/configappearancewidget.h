/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
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

#ifndef CONFIGAPPEARANCEWIDGET_H
#define CONFIGAPPEARANCEWIDGET_H

#include <QWidget>

class QGroupBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTableWidget;
class QTableWidgetItem;
class QTextCharFormat;

class ConfigAppearanceWidget : public QWidget
{
	Q_OBJECT

public:
	ConfigAppearanceWidget(QWidget *parent = 0);
	void addItem(const QString &titleName);
	void addItemFont(const QString &fontName);
	void addItemColor(const QString &colorName);
	void addItems(const QStringList &titleNames);
	void setItemFonts(const QStringList &fontNames);
	void setItemColors(const QStringList &colorNames);
	void setCustomizationType(bool custom);
	QStringList getItemFonts();
	QStringList getItemColors();
	bool getCustomizationType();
	void setTypeNames(const QStringList &typeNames);
	void setDefaultTextCharFormats(const QMap<QString, QTextCharFormat> &defaultFormatList);
	void readSettings(const QString &settingsGroup);
	void setSettings();
	void writeSettings(const QString &settingsGroup);

private:
	void showEvent(QShowEvent*);
	void setHighlightedForeground(const QString &colorName);
	void setItemToolTip(QTableWidgetItem *item, const QFont &font);

	QGroupBox *m_appearanceWidget;
	QRadioButton *m_standardAppearanceCheck;
	QRadioButton *m_customAppearanceCheck;
	QTableWidget *m_itemTable;
	int m_itemHighlighted;
	int m_itemMargin;
	QPushButton *m_fontButton;
	QPushButton *m_colorButton;
	bool m_custom;
	QStringList m_itemFonts;
	QStringList m_itemColors;
	QStringList m_typeNames;

private slots:
	void setItemHighlighted(QTableWidgetItem *item);
	void toggleCustom();
	void showFontDialog();
	void showColorDialog();
};

#endif
