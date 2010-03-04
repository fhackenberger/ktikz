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

#include "configdialog.h"

#include "configgeneralwidget.h"

PartConfigDialog::PartConfigDialog(QWidget* parent)
    : KDialog(parent)
{
	setCaption(i18nc("@title:window", "Configure TikZ Viewer"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::Default);
	showButtonSeparator(true);

	m_configGeneralWidget = new PartConfigGeneralWidget(this);
	setMainWidget(m_configGeneralWidget);

	connect(this, SIGNAL(applyClicked()), this, SLOT(writeSettings()));
	connect(this, SIGNAL(okClicked()), this, SLOT(writeSettings()));
	connect(this, SIGNAL(defaultClicked()), this, SLOT(setDefaults()));
	connect(m_configGeneralWidget, SIGNAL(changed(bool)), this, SLOT(enableButtonApply(bool)));
	enableButtonApply(false);
}

PartConfigDialog::~PartConfigDialog()
{
}

void PartConfigDialog::setDefaults()
{
	m_configGeneralWidget->setDefaults();
}

void PartConfigDialog::readSettings()
{
	m_configGeneralWidget->readSettings();
}

void PartConfigDialog::writeSettings()
{
	m_configGeneralWidget->writeSettings();
	enableButtonApply(false);
	emit settingsChanged("preferences");
}
