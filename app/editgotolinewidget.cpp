/***************************************************************************
 *   Copyright (C) 2008, 2011, 2014 by Glad Deschrijver                    *
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

#include "editgotolinewidget.h"

#include <QtGui/QKeyEvent>
#include "../common/utils/icon.h"

GoToLineWidget::GoToLineWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    ui.spinBoxGo->setMinimum(1);
    ui.pushButtonClose->setIcon(Icon(QLatin1String("dialog-cancel")));

    setFocusProxy(ui.spinBoxGo);

    connect(ui.pushButtonGo, &QAbstractButton::clicked, this, [this]() { goToLine(); });
    connect(ui.pushButtonClose, &QAbstractButton::clicked, this, &GoToLineWidget::hide);
}

GoToLineWidget::~GoToLineWidget() { }

void GoToLineWidget::goToLine()
{
    Q_EMIT goToLine(ui.spinBoxGo->value() - 1);
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
    Q_EMIT focusEditor();
}

void GoToLineWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        hide();
    else if (event->key() == Qt::Key_Return)
        goToLine();
    QWidget::keyPressEvent(event);
}
