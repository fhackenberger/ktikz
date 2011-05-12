/***************************************************************************
 *   Copyright (C) 2007, 2011 by Glad Deschrijver                          *
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

#ifndef INDENTWIDGET_H
#define INDENTWIDGET_H

#include "ui_editindentwidget.h"

class IndentWidget : public QWidget
{
	Q_OBJECT

public:
	explicit IndentWidget(QWidget *parent = 0);
	~IndentWidget();

	void setUnindenting(bool isUnindenting = true);
	QChar insertChar() const;
	int numOfInserts() const;

signals:
	void indent(QChar insertChar, int numOfInserts, bool isUnindenting);
	void hidden();

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

private slots:
	void indent();
	void hide();

private:
	void readSettings();
	void writeSettings();

	Ui::IndentWidget ui;
	bool m_isUnindenting;
};

#endif
