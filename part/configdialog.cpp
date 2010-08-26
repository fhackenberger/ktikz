/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Glad Deschrijver                    *
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

#include <QCheckBox>
#include <QSettings>

#include "configgeneralwidget.h"

PartConfigDialog::PartConfigDialog(QWidget *parent)
    : KDialog(parent)
{
	setCaption(i18nc("@title:window", "Configure KTikZ Viewer"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::Default);
	showButtonSeparator(true);

	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	m_configGeneralWidget = new PartConfigGeneralWidget(this);
	mainLayout->addWidget(viewerWidget());
	mainLayout->addWidget(m_configGeneralWidget);
	setMainWidget(mainWidget);

	connect(this, SIGNAL(applyClicked()), this, SLOT(writeSettings()));
	connect(this, SIGNAL(okClicked()), this, SLOT(writeSettings()));
	connect(this, SIGNAL(defaultClicked()), this, SLOT(setDefaults()));
	connect(m_configGeneralWidget, SIGNAL(changed(bool)), this, SLOT(enableButtonApply(bool)));
	enableButtonApply(false);
}

PartConfigDialog::~PartConfigDialog()
{
}

QWidget *PartConfigDialog::viewerWidget()
{
	QGroupBox *viewerGroupBox = new QGroupBox(i18nc("@title:group", "Viewer"));
	QVBoxLayout *viewerLayout = new QVBoxLayout(viewerGroupBox);
	m_watchFileCheckBox = new QCheckBox(i18nc("@option:check", "&Reload document on file change"));
	m_watchFileCheckBox->setObjectName("watchFileCheckBox");
	m_watchFileCheckBox->setWhatsThis(i18nc("@info:whatsthis", "<para>When this option is checked, "
	    "the TikZ image will be reloaded each time that the file is modified "
	    "by another program.</para>"));
	viewerLayout->addWidget(m_watchFileCheckBox);

	connect(m_watchFileCheckBox, SIGNAL(toggled(bool)), this, SLOT(setModified()));

	return viewerGroupBox;
}

void PartConfigDialog::setDefaults()
{
	m_configGeneralWidget->setDefaults();
	m_watchFileCheckBox->setChecked(true);
}

void PartConfigDialog::readSettings()
{
	m_configGeneralWidget->readSettings();

	QSettings settings(ORGNAME, APPNAME);
	m_watchFileCheckBox->setChecked(settings.value("WatchFile", true).toBool());
}

void PartConfigDialog::setModified()
{
	QWidget *sendingWidget = qobject_cast<QWidget*>(sender());
	QSettings settings(ORGNAME, APPNAME);
	if (sendingWidget->objectName() == QLatin1String("watchFileCheckBox"))
		enableButtonApply(m_watchFileCheckBox->isChecked() != settings.value("WatchFile", true).toBool());
}

void PartConfigDialog::writeSettings()
{
	m_configGeneralWidget->writeSettings();

	QSettings settings(ORGNAME, APPNAME);
	settings.setValue("WatchFile", m_watchFileCheckBox->isChecked());

	enableButtonApply(false);
	emit settingsChanged("preferences");
}
