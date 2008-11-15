/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
 *   Glad.Deschrijver@UGent.be                                             *
 *                                                                         *
 *   Original code from SpeedCrunch                                        *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QApplication>
#include <QColorDialog>
#include <QPainter>
#include <QStyle>
#include "colorbutton.h"

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

QColor ColorButton::getColor() const
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
