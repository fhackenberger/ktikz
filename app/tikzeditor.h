/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011 by Glad Deschrijver              *
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

#include <QtGui/QPlainTextEdit>

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
	void setShowWhiteSpaces(bool visible);
	void setShowTabulators(bool visible);
	void setShowMatchingBrackets(bool visible);
	void setWhiteSpacesColor(const QColor &color);
	void setTabulatorsColor(const QColor &color);
	void setMatchingColor(const QColor &color);
	void goToLine(int line);
	int numOfLines() const;
	void updateCompleter(bool useCompletion, const QStringList &words);

	void toggleUserBookmark(int lineNumber);
	int userBookmark(int which) const;
	QList<int> userBookmarks() const;
	void setUserBookmarks(const QList<int> &bookmarks);

	void setShowLineNumberArea(bool visible);
	friend class LineNumberWidget;

public slots:
	void showCursorPosition();
	void toggleUserBookmark();
	void previousUserBookmark();
	void nextUserBookmark();

signals:
	void cursorPositionChanged(int row, int col);
	void showStatusMessage(const QString &message, int timeout = 3000);
	void focusIn();
	void focusOut();
	void tabIndent(bool isUnindenting);

protected:
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void focusInEvent(QFocusEvent *event);
	void focusOutEvent(QFocusEvent *event);
	int lineNumberAreaWidth();
	void resizeEvent(QResizeEvent *event);

private slots:
	void highlightCurrentLine();
	void matchBrackets();
	void insertCompletion(const QString &completion);
	void recalculateBookmarks(int position);
	void updateLineNumberAreaWidth();
	void updateLineNumberArea(const QRect &rect, int dy);

private:
	void showMatchingBrackets();
	void paintTabstop(QPainter &painter, qreal x, qreal y, int spaceWidth);
	void paintSpace(QPainter &painter, qreal x, qreal y, int spaceWidth);
	void printWhiteSpaces(QPainter &painter);
	QString textUnderCursor() const;
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

	QList<int> m_userBookmarks;
	int m_oldNumOfLines;

	QWidget *m_lineNumberArea;
	bool m_showLineNumberArea;
};

#endif
