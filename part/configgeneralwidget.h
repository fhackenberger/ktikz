/***************************************************************************
 *   Copyright (C) 2008, 2009 by Glad Deschrijver                          *
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

#ifndef KTIKZ_PART_CONFIGGENERALWIDGET_H
#define KTIKZ_PART_CONFIGGENERALWIDGET_H

#include <QWidget>
#include "ui_configgeneralwidget.h"

namespace KtikZ
{

class PartConfigGeneralWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PartConfigGeneralWidget(QWidget *parent = 0);

	void setDefaults();
	void readSettings(const QString &settingsGroup = QString());
	void writeSettings(const QString &settingsGroup = QString());

signals:
	void changed(bool isChanged);

protected:
	Ui::PartConfigGeneralWidget ui;

private slots:
	void setModified();
};

} // namespace KtikZ

#endif
