/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
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

#ifndef GOTOLINEWIDGET_H
#define GOTOLINEWIDGET_H

#include "ui_editgotolinewidget.h"

class GoToLineWidget : public QWidget
{
	Q_OBJECT

public:
	GoToLineWidget(QWidget *parent = 0);
	~GoToLineWidget();
	void setMaximumValue(int maximumValue);
	void setValue(int value);

public slots:
	void goToLine();

signals:
	void goToLine(int lineNumber);
	void focusEditor();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

protected slots:
	void hide();

private:
	Ui::GoToLineWidget ui;
};

#endif
