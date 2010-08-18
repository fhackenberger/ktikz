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

#include "logtextedit.h"

#include <QApplication>

LogTextEdit::LogTextEdit(QWidget *parent) : QTextEdit(parent)
{
}

QSize LogTextEdit::sizeHint() const
{
	return QSize(300, 90);
}

void LogTextEdit::logUpdated(const QString &logText)
{
	setPlainText(logText);
}

void LogTextEdit::logUpdated(const QString &logText, bool runFailed)
{
	setPlainText(logText);
	setLogPalette(runFailed);
}

void LogTextEdit::setLogPalette(bool runFailed)
{
    moveCursor(QTextCursor::End);
    if (runFailed)
    {
        QPalette failedPalette(QApplication::palette());
        failedPalette.setColor(QPalette::Background, QColor(255, 102, 102));
        setAutoFillBackground(true);
        setPalette(failedPalette);
    }
    else
        setPalette(QApplication::palette());
}
