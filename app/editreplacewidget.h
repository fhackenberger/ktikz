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

#ifndef REPLACEWIDGET_H
#define REPLACEWIDGET_H

#include "ui_editreplacewidget.h"

class ReplaceWidget : public QWidget
{
	Q_OBJECT

public:
	ReplaceWidget(QWidget *parent = 0);
	~ReplaceWidget();
	void setForward(bool forward);
	void setText(const QString &text);

public slots:
	void doFind();

signals:
	void focusEditor();
	void search(const QString &text, bool isCaseSensitive, bool findWholeWords, bool forward);
	void replace(const QString &text, const QString &replacement, bool isCaseSensitive, bool findWholeWords, bool forward);

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

protected slots:
	void hide();

private:
	void doFind(bool forward);

	Ui::ReplaceWidget ui;

private slots:
	void setBackward();
	void setForward();
	void doReplace();
};

#endif
