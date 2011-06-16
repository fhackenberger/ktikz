/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2011 by Glad Deschrijver              *
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

#include "configdialog.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QWhatsThis>

#include "configgeneralwidget.h"
#include "configeditorwidget.h"
#include "configappearancewidget.h"
#include "ktikzapplication.h"
#include "../common/utils/icon.h"

ConfigDialog::ConfigDialog(QWidget *parent) : PageDialog(parent)
{
	setCaption(tr("Configure %1").arg(KtikzApplication::applicationName()));
	setHelp("chap-configuration");

	m_configGeneralWidget = new ConfigGeneralWidget(this);
	m_configEditorWidget = new ConfigEditorWidget(this);
	m_configAppearanceWidget = new ConfigAppearanceWidget(this);
	addPage(m_configGeneralWidget, tr("&General"), "preferences-desktop-theme");
	addPage(m_configEditorWidget, tr("&Editor"), "accessories-text-editor");
	addPage(m_configAppearanceWidget, tr("&Highlighting"), "preferences-desktop-color");
}

void ConfigDialog::readSettings()
{
	m_configGeneralWidget->readSettings("");
	m_configEditorWidget->readSettings("Editor");
	m_configAppearanceWidget->readSettings("Highlighting");
}

void ConfigDialog::writeSettings()
{
	m_configGeneralWidget->writeSettings("");
	m_configEditorWidget->writeSettings("Editor");
	m_configAppearanceWidget->writeSettings("Highlighting");
}

void ConfigDialog::setTranslatedHighlightTypeNames(const QStringList &typeNames)
{
	for (int i = 0; i < typeNames.size(); ++i)
		m_configAppearanceWidget->addItem(typeNames.at(i));
}

void ConfigDialog::setHighlightTypeNames(const QStringList &typeNames)
{
	m_configAppearanceWidget->setTypeNames(typeNames);
}

void ConfigDialog::setDefaultHighlightFormats(const QMap<QString, QTextCharFormat> &defaultFormatList)
{
	m_configAppearanceWidget->setDefaultTextCharFormats(defaultFormatList);
}

void ConfigDialog::accept()
{
	writeSettings();
	emit settingsChanged();
	QDialog::accept();
}

void ConfigDialog::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_F1 && event->modifiers() == Qt::ShiftModifier)
		QWhatsThis::enterWhatsThisMode();
	QDialog::keyPressEvent(event);
}
