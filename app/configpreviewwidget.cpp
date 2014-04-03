/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014 by Glad Deschrijver  *
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

#include "configpreviewwidget.h"

#include <QtCore/QSettings>

ConfigPreviewWidget::ConfigPreviewWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ConfigPreviewWidget::~ConfigPreviewWidget()
{
}

void ConfigPreviewWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	if (settings.value("BuildAutomatically", true).toBool())
		ui.buildAutomaticallyRadio->setChecked(true);
	else
		ui.buildManuallyRadio->setChecked(true);

	ui.showCoordinatesCheck->setChecked(settings.value("ShowCoordinates", true).toBool());
	const int precision = settings.value("ShowCoordinatesPrecision", -1).toInt();
	if (precision < 0)
	{
		ui.bestPrecisionRadio->setChecked(true);
	}
	else
	{
		ui.specifyPrecisionRadio->setChecked(true);
		ui.specifyPrecisionSpinBox->setValue(precision);
	}
	settings.endGroup();
}

void ConfigPreviewWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	settings.setValue("BuildAutomatically", ui.buildAutomaticallyRadio->isChecked());
	settings.setValue("ShowCoordinates", ui.showCoordinatesCheck->isChecked());
	if (ui.bestPrecisionRadio->isChecked())
		settings.setValue("ShowCoordinatesPrecision", -1);
	else
		settings.setValue("ShowCoordinatesPrecision", ui.specifyPrecisionSpinBox->value());
	settings.endGroup();
}
