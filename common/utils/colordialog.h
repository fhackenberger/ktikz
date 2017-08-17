/***************************************************************************
 *   Copyright (C) 2009, 2014 by Glad Deschrijver                          *
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

#ifndef KTIKZ_COLORDIALOG_H
#define KTIKZ_COLORDIALOG_H

#include <QColorDialog>

class ColorDialog : public QColorDialog
{
public:
	// Commented out for Qt 4.5 compatibility (should not be required,
	// as we don't instantiate ColorDialog directly anyway
	//explicit ColorDialog(QWidget *parent) : QColorDialog(parent) {}

	static QColor getColor(bool *ok, const QColor &color, QWidget *parent)
	{
		const QColor newColor = QColorDialog::getColor(color, parent);
		*ok = newColor.isValid();
		return newColor;
	}
};

#endif
