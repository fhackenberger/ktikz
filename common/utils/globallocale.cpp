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

#include "globallocale.h"

#ifdef KTIKZ_USE_KDE
#include <KGlobal>
#include <KLocale>

QString GlobalLocale::decimalSymbol()
{
	return KGlobal::locale()->decimalSymbol();
}

QString GlobalLocale::formatNumber(double num, int precision)
{
	return KGlobal::locale()->formatNumber(num, precision);
}

double GlobalLocale::readNumber(const QString &str)
{
	return KGlobal::locale()->readNumber(str);
}
#else
#include <QtCore/QLocale>

QString GlobalLocale::decimalSymbol()
{
	return QLocale::system().decimalPoint();
}

QString GlobalLocale::formatNumber(double num, int precision)
{
	return QLocale::system().toString(num, 'f', precision);
}

double GlobalLocale::readNumber(const QString &str)
{
	return str.toDouble();
}
#endif
