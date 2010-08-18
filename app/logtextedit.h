/***************************************************************************
 *   Copyright (C) 2007, 2008 by Glad Deschrijver                          *
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

#ifndef LOGTEXTEDIT_H
#define LOGTEXTEDIT_H

#include <QTextEdit>

class LogTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	LogTextEdit(QWidget *parent = 0);
	virtual QSize sizeHint() const;

public slots:
	void logUpdated(const QString &logText);
	void logUpdated(const QString &logText, bool runFailed);

private:
	void setLogPalette(bool runFailed);
};

#endif
