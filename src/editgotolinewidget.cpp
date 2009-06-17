/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
 *   glad.deschrijver@gmail.com                                            *
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

#ifdef KTIKZ_USE_KDE
#include <KIcon>
#endif

#include <QKeyEvent>
#include "editgotolinewidget.h"

GoToLineWidget::GoToLineWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.spinBoxGo->setMinimum(1);
#ifdef KTIKZ_USE_KDE
	ui.pushButtonClose->setIcon(KIcon("window-close"));
#endif

	setFocusProxy(ui.spinBoxGo);

	connect(ui.pushButtonGo, SIGNAL(clicked()), this, SLOT(goToLine()));
	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(hide()));
}

GoToLineWidget::~GoToLineWidget()
{
}

void GoToLineWidget::goToLine()
{
	emit goToLine(ui.spinBoxGo->value() - 1);
}

void GoToLineWidget::setMaximumValue(int maximumValue)
{
	ui.spinBoxGo->setMaximum(maximumValue);
}

void GoToLineWidget::setValue(int value)
{
	ui.spinBoxGo->setValue(value);
	ui.spinBoxGo->setFocus();
	ui.spinBoxGo->selectAll();
}

void GoToLineWidget::hide()
{
	setVisible(false);
	emit focusEditor();
}

void GoToLineWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
		hide();
	else if (event->key() == Qt::Key_Return)
		goToLine();
	QWidget::keyPressEvent(event);
}
