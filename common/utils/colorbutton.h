/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
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

#ifndef KTIKZ_COLORBUTTON_H
#define KTIKZ_COLORBUTTON_H

#ifdef KTIKZ_USE_KDE
#include <KColorButton>

class ColorButton : public KColorButton
{
	Q_OBJECT

public:
	explicit ColorButton(QWidget *parent = 0) : KColorButton(parent) {}
	explicit ColorButton(const QColor &color, QWidget *parent = 0) : KColorButton(color, parent) {}
};
#else
#include <QToolButton>

class ColorButton : public QToolButton
{
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor)

public:
	explicit ColorButton(QWidget *parent = 0);
	explicit ColorButton(const QColor &color, QWidget *parent = 0);

	QColor color() const;
	void setColor(const QColor &color);

signals:
	void colorChanged();

protected:
	void paintEvent(QPaintEvent*);

private:
	QColor m_color;

private slots:
	void showColorDialog();
};
#endif

#endif
