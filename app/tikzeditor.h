/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Glad Deschrijver                    *
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

#ifndef TIKZEDITOR_H
#define TIKZEDITOR_H

#include <QPlainTextEdit>

class QCompleter;
class QLabel;
class QString;

class TikzEditor : public QPlainTextEdit
{
	Q_OBJECT
	Q_PROPERTY(int numOfLines READ numOfLines)

public:
	TikzEditor(QWidget *parent = 0);
	~TikzEditor();
	void setShowWhiteSpaces(bool show);
	void setShowTabulators(bool show);
	void setShowMatchingBrackets(bool show);
	void setWhiteSpacesColor(const QColor &color);
	void setTabulatorsColor(const QColor &color);
	void setMatchingColor(const QColor &color);
	void goToLine(int line);
	int numOfLines() const;
	void setCompleter(QCompleter *completer);

public slots:
	void showCursorPosition();

signals:
	void cursorPositionChanged(int row, int col);
	void showStatusMessage(const QString &message, int timeout = 3000);
	void focusIn();
	void focusOut();

protected:
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void focusInEvent(QFocusEvent *event);
	void focusOutEvent(QFocusEvent *event);

private:
	void showMatchingBrackets();
	uint spaceWidth() const;
	void paintTabstop(QPainter &painter, qreal x, qreal y);
	void paintSpace(QPainter &painter, qreal x, qreal y);
	void printWhiteSpaces(QPainter &painter);
	QString textUnderCursor() const;
	int getCursorPosition(int row, int col) const;
	void setCursorPosition(int row, int col);

	QColor m_highlightCurrentLineColor;
	QRect m_previousHighlightedLine;
	int m_oldVerticalScrollBarValue;

	QString m_plainText;
	QColor m_whiteSpacesColor;
	QColor m_tabulatorsColor;
	QColor m_matchingColor;
	long m_matchingBegin, m_matchingEnd;
	bool m_showWhiteSpaces;
	bool m_showTabulators;
	bool m_showMatchingBrackets;

	QCompleter *m_completer;

private slots:
	void highlightCurrentLine();
	void matchBrackets();
	void insertCompletion(const QString &completion);
};

#endif
