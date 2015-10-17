/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2011, 2014                            *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
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

#include <QKeyEvent>
#include <QWhatsThis>
#include <KConfigGroup>

#include "configappearancewidget.h"
#include "configeditorwidget.h"
#include "configgeneralwidget.h"
#include "configpreviewwidget.h"
#include "ktikzapplication.h"
#include "../common/utils/icon.h"

ConfigDialog::ConfigDialog(QWidget *parent) : PageDialog(parent)
{
	setWindowTitle(tr("Configure %1").arg(KtikzApplication::applicationName()));
// TODO: Check if this is needed
// 	setHelp(QLatin1String("chap-configuration"));

	m_configGeneralWidget = new ConfigGeneralWidget(this);
	m_configEditorWidget = new ConfigEditorWidget(this);
	m_configAppearanceWidget = new ConfigAppearanceWidget(this);
	m_configPreviewWidget = new ConfigPreviewWidget(this);
	addPage(m_configGeneralWidget, tr("&General"), QLatin1String("preferences-desktop-theme"));
	addPage(m_configEditorWidget, tr("&Editor"), QLatin1String("accessories-text-editor"));
	addPage(m_configAppearanceWidget, tr("&Highlighting"), QLatin1String("preferences-desktop-color"));
	addPage(m_configPreviewWidget, tr("&Preview"), QLatin1String("preferences-desktop-theme"));
}

void ConfigDialog::readSettings()
{
	m_configGeneralWidget->readSettings(QString());
	m_configEditorWidget->readSettings(QLatin1String("Editor"));
	m_configAppearanceWidget->readSettings(QLatin1String("Highlighting"));
	m_configPreviewWidget->readSettings(QLatin1String("Preview"));
}

void ConfigDialog::writeSettings()
{
	m_configGeneralWidget->writeSettings(QString());
	m_configEditorWidget->writeSettings(QLatin1String("Editor"));
	m_configAppearanceWidget->writeSettings(QLatin1String("Highlighting"));
	m_configPreviewWidget->writeSettings(QLatin1String("Preview"));
}

void ConfigDialog::setTranslatedHighlightTypeNames(const QStringList &typeNames)
{
	Q_FOREACH (const QString &typeName, typeNames)
		m_configAppearanceWidget->addItem(typeName);
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
	Q_EMIT settingsChanged();
	QDialog::accept();
}

void ConfigDialog::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_F1 && event->modifiers() == Qt::ShiftModifier)
		QWhatsThis::enterWhatsThisMode();
	QDialog::keyPressEvent(event);
}
