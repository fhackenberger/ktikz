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

#ifndef KTIKZ_CONFIGDIALOG_H
#define KTIKZ_CONFIGDIALOG_H

#ifdef KTIKZ_USE_KDE
#include <KPageDialog>
#else
#include <QDialog>
#endif

class QLabel;
class QListWidgetItem;
//class QTabWidget;
class QTextCharFormat;

class ConfigGeneralWidget;
class ConfigEditorWidget;
class ConfigAppearanceWidget;

#ifdef KTIKZ_USE_KDE
class ConfigDialog : public KPageDialog
#else
class ConfigDialog : public QDialog
#endif
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = 0);

	void readSettings();
	void writeSettings();

	void setTranslatedHighlightTypeNames(const QStringList &typeNames);
	void setHighlightTypeNames(const QStringList &typeNames);
	void setDefaultHighlightFormats(const QMap<QString, QTextCharFormat> &defaultFormatList);

signals:
	void settingsChanged();

private:
#ifndef KTIKZ_USE_KDE
	QWidget *centerWidget();
#endif
	void addPage(QWidget *widget, const QString &title, const QString &iconName = 0);
	QWidget *generalPage();
	QWidget *editorPage();
	QWidget *appearancePage();

	void keyPressEvent(QKeyEvent *event);

//	QTabWidget *m_pagesTabWidget;
//	QStringList m_pageTitles;

#ifndef KTIKZ_USE_KDE
	QList<QListWidgetItem*> m_pagesListWidgetItems;
	QLabel *m_pagesTitleLabel;
	QList<QWidget*> m_pageWidgets;
#endif
	ConfigGeneralWidget *m_configGeneralWidget;
	ConfigEditorWidget *m_configEditorWidget;
	ConfigAppearanceWidget *m_configAppearanceWidget;

private slots:
#ifndef KTIKZ_USE_KDE
	void setCurrentPage(int page);
#endif
	void accept();
};

#endif
