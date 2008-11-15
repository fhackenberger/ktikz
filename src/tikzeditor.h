/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
 *   Glad.Deschrijver@UGent.be                                             *
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

#ifndef TIKZEDITOR_H
#define TIKZEDITOR_H

#include <QTextEdit>

class QCompleter;
class QLabel;
class QString;

class TikzEditor : public QTextEdit
{
	Q_OBJECT

public:
	TikzEditor(QWidget *parent = 0);
	~TikzEditor();
	void setShowMatchingBrackets(bool showMatchingBrackets);
	void setMatchingColor(const QColor &matchingColor);
	void setShowWhiteSpaces(bool showWhiteSpaces);
	void goToLine(int line);
	int getCursorPosition(int row, int col) const;
	void setCursorPosition(int row, int col);
	int numOfLines() const;
	void setCompleter(QCompleter *completer);

public slots:
	void showCursorPosition();

private:
	QString textUnderCursor() const;

	QString m_plainText;
	long m_matchingBegin, m_matchingEnd;
	QColor m_matchingColor;
	bool m_showMatchingBrackets;

	void printWhiteSpaces(QPainter &painter);
	QPixmap m_tabPixmap, m_spacePixmap;
	bool m_showWhiteSpaces;

	QCompleter *m_completer;

private slots:
	void matchBrackets();
	void insertCompletion(const QString &completion);

protected:
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void focusInEvent(QFocusEvent *event);

signals:
	void cursorPositionChanged(int row, int col);
};

#endif
