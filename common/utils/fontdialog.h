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

#ifndef KTIKZ_FONTDIALOG_H
#define KTIKZ_FONTDIALOG_H

#ifdef KTIKZ_USE_KDE
#include <KFontDialog>

class FontDialog : public KFontDialog
{
public:
	FontDialog(QWidget *parent) : KFontDialog(parent) {}

	static QFont getFont(bool *ok, const QFont &font, QWidget *parent)
	{
		Q_UNUSED(parent);
		QFont newFont = font;
		const int result = KFontDialog::getFont(newFont);
		*ok = (result == KFontDialog::Accepted);
		return newFont;
	}
};
#else
#include <QFontDialog>

class FontDialog : public QFontDialog
{
public:
	// Commented out for Qt 4.5 compatibility (should not be required,
	// as we don't instantiate FontDialog directly anyway).
	//FontDialog(QWidget *parent) : QFontDialog(parent) {}

	static QFont getFont(bool *ok, const QFont &font, QWidget *parent)
	{
		return QFontDialog::getFont(ok, font, parent);
	}
};
#endif

#endif
