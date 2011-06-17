/***************************************************************************
 *   Copyright (C) 2011 by Glad Deschrijver                                *
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

#ifndef KTIKZ_MESSAGEBOX_H
#define KTIKZ_MESSAGEBOX_H

#include <QtCore/QString>
class QWidget;

class MessageBox
{
public:
	enum ButtonCode
	{
		Ok = 1,
		Cancel = 2,
		Yes = 3,
		No = 4,
		Continue = 5
	};

	static int questionYesNo(QWidget *parent, const QString &text, const QString &caption = QString(), const QString &yesButtonText = QString(), const QString &noButtonText = QString());
	static void sorry(QWidget *parent, const QString &text, const QString &caption = QString());
	static void error(QWidget *parent, const QString &text, const QString &caption = QString());
};

#endif
