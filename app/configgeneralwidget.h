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

#ifndef KTIKZ_CONFIGGENERALWIDGET_H
#define KTIKZ_CONFIGGENERALWIDGET_H

#include <QWidget>
#include "ui_configgeneralwidget.h"

class ConfigGeneralWidget : public QWidget
{
	Q_OBJECT

public:
	ConfigGeneralWidget(QWidget *parent = 0);

	void readSettings(const QString &settingsGroup);
	void writeSettings(const QString &settingsGroup);

protected:
	Ui::ConfigGeneralWidget ui;

private slots:
	void searchTikzDocumentation();
	void browseCommand();

private:
	void browseCommand(QLineEdit *lineEdit, bool isProgram = true);
};

#endif
