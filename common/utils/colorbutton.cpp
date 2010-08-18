/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   Original code from SpeedCrunch:                                       *
 *     Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>                    *
 *     (http://code.google.com/p/speedcrunch/)                             *
 *     licensed under GPL v2 or later                                      *
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

#include "colorbutton.h"

#ifndef KTIKZ_USE_KDE
#include <QApplication>
#include <QColorDialog>
#include <QPainter>
#include <QStyle>

ColorButton::ColorButton(QWidget *parent) : QToolButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(showColorDialog()));
}

ColorButton::ColorButton(const QColor &color, QWidget *parent) : QToolButton(parent)
{
	setColor(color);
	connect(this, SIGNAL(clicked()), this, SLOT(showColorDialog()));
}

void ColorButton::showColorDialog()
{
	const QColor newColor = QColorDialog::getColor(m_color, this);
	if (newColor.isValid())
		setColor(newColor);
}

QColor ColorButton::color() const
{
	return m_color;
}

void ColorButton::setColor(const QColor &color)
{
	m_color = color;
	emit colorChanged();
	update();
}

void ColorButton::paintEvent(QPaintEvent *e)
{
	QToolButton::paintEvent(e);
	if (!isEnabled())
		return;

	QRect r = rect();
	r.adjust(5, 5, -5, -5);
	QPainter painter(this);
	const QColor borderColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Dark));
	painter.setPen(borderColor);
	painter.drawRect(r);
	r.adjust(1, 1, 0, 0);
	painter.fillRect(r, m_color);
}
#endif
