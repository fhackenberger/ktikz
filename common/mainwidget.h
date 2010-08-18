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

#ifndef KTIKZ_MAIN_WIDGET_H
#define KTIKZ_MAIN_WIDGET_H

#include <QString>
#include <QWidget>

#include "utils/url.h"

class MainWidget
{
public:
	virtual ~MainWidget() {}

	virtual QWidget *widget()
	{
		return new QWidget();
	}
	virtual QString tikzCode() const
	{
		return QString();
	}
	virtual Url url() const
	{
		return Url();
	}
};

#endif
