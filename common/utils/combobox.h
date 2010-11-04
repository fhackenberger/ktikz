/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#ifndef KTIKZ_COMBOBOX_H
#define KTIKZ_COMBOBOX_H

#ifdef KTIKZ_USE_KDE
#include <KComboBox>

class ComboBox : public KComboBox
{
public:
	ComboBox(QWidget *parent = 0) : KComboBox(parent) {}
};
#else
#include <QtGui/QComboBox>

class ComboBox : public QComboBox
{
public:
	ComboBox(QWidget *parent = 0) : QComboBox(parent) {}
};
#endif

#endif
