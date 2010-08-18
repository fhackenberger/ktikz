/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Glad Deschrijver                    *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   Bracket matching and white space showing code originally from         *
 *     qdevelop: Copyright (C) 2006 Jean-Luc Biord <jlbiord@gmail.com>     *
 *     (http://qdevelop.org) licensed under GPL v2 or later                *
 *   Enhanced version of bracket matching inspired by codeedit example in  *
 *     the Qt documentation: Copyright (C) 2009 Nokia Corporation and/or   *
 *     its subsidiary(-ies) (qt-info@nokia.com) licensed under LGPL v2.1   *
 *   Enhanced white space and tab marking code based on katerenderer which *
 *     is part of the KDE libraries:                                       *
 *     Copyright (C) 2007 Mirko Stocker <me@misto.ch>,                     *
 *     Copyright (C) 2003-2005 Hamish Rodda <rodda@kde.org>,               *
 *     Copyright (C) 2001 Christoph Cullmann <cullmann@kde.org>,           *
 *     Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>,               *
 *     Copyright (C) 1999 Jochen Wilhelmy <digisnap@cs.tu-berlin.de>       *
 *     licensed under LGPL v2                                              *
 *   Completion code originally from texmaker:                             *
 *     Copyright (C) 2003-2008 Pascal Brachet <pbrachet@xm1math.net>       *
 *     (http://www.xm1math.net/texmaker) licensed under GPL v2 or later    *
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

#include "tikzeditor.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QCompleter>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextLayout>

static const QString s_completionPlaceHolder(0x2022);

TikzEditor::TikzEditor(QWidget *parent) : QPlainTextEdit(parent)
{
	m_showWhiteSpaces = true;
	m_showTabulators = true;
	m_showMatchingBrackets = true;
	m_whiteSpacesColor = Qt::gray;
	m_tabulatorsColor = Qt::gray;
	m_matchingColor = Qt::yellow;

	m_completer = 0;

//	setLineWidth(0);
//	setFrameShape(QFrame::NoFrame);

	const QColor lineColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Base));
	const QColor altLineColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::AlternateBase));
	if (lineColor == altLineColor)
		m_highlightCurrentLineColor = lineColor.darker(105);
	else
		m_highlightCurrentLineColor = altLineColor;
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	highlightCurrentLine();

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(showCursorPosition()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchBrackets()));
}

TikzEditor::~TikzEditor()
{
}

void TikzEditor::highlightCurrentLine()
{
/*
	// this hides the white space and tab marks on the current line :-(
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(m_highlightCurrentLineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);

	// highlight the currently selected brackets (if any)
	matchBrackets();
*/

	// update the area covering the previously highlighted line (updating the whole viewport is too slow :-( )
	m_previousHighlightedLine.moveTop(m_previousHighlightedLine.top() - (verticalScrollBar()->value() - m_oldVerticalScrollBarValue + 0.5) * m_previousHighlightedLine.height());
	m_previousHighlightedLine.setHeight(2 * m_previousHighlightedLine.height()); // should be large enough to cover the previous line (ugly hack :-( )
	viewport()->update(m_previousHighlightedLine);

	// update the area covering the currently highlighted line
	QRect rect = cursorRect();
	rect.setX(0);
	rect.setWidth(viewport()->width());
	viewport()->update(rect);

	m_previousHighlightedLine = rect;
	m_oldVerticalScrollBarValue = verticalScrollBar()->value();
}

void TikzEditor::matchBrackets()
{
	// clear previous bracket highlighting
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);

	// find current matching brackets
	m_matchingBegin = -1;
	m_matchingEnd = -1;
	if (!m_showMatchingBrackets) return;

	m_plainText = toPlainText();
// 	QString matchText = simplifiedText(plainText);
	const QString matchText = m_plainText;
	const QTextCursor cursor = textCursor();
	int pos = cursor.position();
	if (pos == -1)
		return;
	else if (cursor.atEnd() || !QString("({[]})").contains(m_plainText.at(pos))) // if the cursor is not next to a bracket, then there is nothing to match, so return
	{
		if (pos <= 0 || !QString("({[]})").contains(m_plainText.at(--pos)))
			return;
	}

	// get corresponding opening/closing bracket and search direction
	QChar car = (!cursor.atEnd()) ? matchText.at(pos) : matchText.at(pos - 1);
	QChar matchCar;
	long inc = 1;
	if (car == '(') matchCar = ')';
	else if (car == '{') matchCar = '}';
	else if (car == '[') matchCar = ']';
	else
	{
		inc = -1;
		if (car == ')') matchCar = '(';
		else if (car == '}') matchCar = '{';
		else if (car == ']') matchCar = '[';
		else
			return;
	}

	// find location of the corresponding bracket
	m_matchingBegin = pos;
	int numOfMatchCharsToSkip = 0;
	do
	{
		if (matchText.at(pos) == car)
			numOfMatchCharsToSkip++;
		else if (matchText.at(pos) == matchCar)
		{
			numOfMatchCharsToSkip--;
			if (numOfMatchCharsToSkip == 0)
			{
				m_matchingEnd = pos;
				break;
			}
		}
		pos += inc;
	}
	while (pos >= 0 && pos < matchText.length());

	if (m_matchingBegin > m_matchingEnd)
		qSwap(m_matchingBegin, m_matchingEnd);

	// if there is a match, then show it
	if (m_matchingBegin != -1)
		showMatchingBrackets();
}

void TikzEditor::showMatchingBrackets()
{
	for (QTextBlock block = firstVisibleBlock(); block.isValid(); block = block.next())
	{
		if (blockBoundingGeometry(block).top() > viewport()->height())
			break;

		const QString text = block.text();
		const int textLength = text.length();

		for (int i = 0; i < textLength; ++i)
		{
			if (block.position() + i == m_matchingBegin || block.position() + i == m_matchingEnd)
			{
				QList<QTextEdit::ExtraSelection> extraSelectionList = extraSelections();
				if (!isReadOnly())
				{
					QTextEdit::ExtraSelection selection;
					selection.format.setBackground(m_matchingColor);
					selection.cursor = textCursor();
					selection.cursor.setPosition(block.position() + i, QTextCursor::MoveAnchor);
					selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
					extraSelectionList.append(selection);
				}
				setExtraSelections(extraSelectionList);
			}
		}
	}
}

void TikzEditor::setShowWhiteSpaces(bool show)
{
	m_showWhiteSpaces = show;
}

void TikzEditor::setShowTabulators(bool show)
{
	m_showTabulators = show;
}

void TikzEditor::setShowMatchingBrackets(bool show)
{
	m_showMatchingBrackets = show;
}

void TikzEditor::setWhiteSpacesColor(const QColor &color)
{
	m_whiteSpacesColor = color;
}

void TikzEditor::setTabulatorsColor(const QColor &color)
{
	m_tabulatorsColor = color;
}

void TikzEditor::setMatchingColor(const QColor &color)
{
	m_matchingColor = color;
}

uint TikzEditor::spaceWidth() const
{
	return QFontMetrics(document()->defaultFont()).width(' ');
}

void TikzEditor::paintTabstop(QPainter &painter, qreal x, qreal y)
{
	QPen penBackup(painter.pen());
	QPen pen(m_tabulatorsColor);
	pen.setWidthF(qMax(0.5, spaceWidth() * .1));
	pen.setCapStyle(Qt::RoundCap);
	painter.setPen(pen);

	// FIXME: optimize for speed!
	qreal dist = spaceWidth() * 0.3;
	QPointF points[8];
	points[0] = QPointF(x - dist, y - dist);
	points[1] = QPointF(x, y);
	points[2] = QPointF(x, y);
	points[3] = QPointF(x - dist, y + dist);
	x += spaceWidth() / 3.0;
	points[4] = QPointF(x - dist, y - dist);
	points[5] = QPointF(x, y);
	points[6] = QPointF(x, y);
	points[7] = QPointF(x - dist, y + dist);
	painter.drawLines(points, 4);
	painter.setPen(penBackup);
}

void TikzEditor::paintSpace(QPainter &painter, qreal x, qreal y)
{
	QPen penBackup(painter.pen());
	QPen pen(m_whiteSpacesColor);
	pen.setWidthF(spaceWidth() / 3.5);
	pen.setCapStyle(Qt::RoundCap);
	painter.setPen(pen);

	painter.drawPoint(QPointF(x, y));
	painter.setPen(penBackup);
}

void TikzEditor::printWhiteSpaces(QPainter &painter)
{
	const QFontMetrics fontMetrics = QFontMetrics(document()->defaultFont());

	for (QTextBlock block = firstVisibleBlock(); block.isValid(); block = block.next())
	{
		if (blockBoundingGeometry(block).top() > viewport()->height())
			break;

		const QString text = block.text();
		const int textLength = text.length();

		for (int i = 0; i < textLength; ++i)
		{
			QTextCursor cursor = textCursor();
			cursor.setPosition(block.position() + i, QTextCursor::MoveAnchor);
			const QRect rect = cursorRect(cursor);

//			const QFontMetrics fontMetrics = QFontMetrics(cursor.charFormat().font());

			if (m_showWhiteSpaces && text.at(i) == ' ')
				paintSpace(painter, rect.x() + spaceWidth() / 2.0, rect.y() + fontMetrics.height() / 2.0);
			else if (m_showTabulators && text.at(i) == '\t')
				paintTabstop(painter, rect.x() + spaceWidth() / 2.0, rect.y() + fontMetrics.height() / 2.0);
		}
	}
}

void TikzEditor::paintEvent(QPaintEvent *event)
{
	QPainter painter(viewport());

	// highlight current line
	QRect rect = cursorRect();
	rect.setX(0);
	rect.setWidth(viewport()->width());
	painter.fillRect(rect, QBrush(m_highlightCurrentLineColor));

	// show white spaces and tabulators
	if (m_showWhiteSpaces || m_showTabulators)
		printWhiteSpaces(painter);

	painter.end();

	QPlainTextEdit::paintEvent(event);
}

void TikzEditor::goToLine(int line)
{
	if (line <= numOfLines())
		setCursorPosition(line, 0);
}

int TikzEditor::getCursorPosition(int row, int col) const
{
	int i = 0;
	QTextBlock p = document()->begin();
	while (p.isValid())
	{
		if (row == i) break;
		i++;
		p = p.next();
	}
	return p.position() + col;
}

void TikzEditor::setCursorPosition(int row, int col)
{
	const int pos = getCursorPosition(row, col);
	QTextCursor cursor = textCursor();
	cursor.setPosition(pos, QTextCursor::MoveAnchor);
	setTextCursor(cursor);

	// make sure that the cursor is in the middle of the visible area
/*
	ensureCursorVisible();
	const int newPosition = verticalScrollBar()->value() + cursorRect().top() - viewport()->height() / 2;
	verticalScrollBar()->setValue(newPosition);
*/
	centerCursor();
	setFocus();
}

int TikzEditor::numOfLines() const
{
	int num = 0;
	QTextBlock p;
	for (p = document()->begin(); p.isValid(); p = p.next())
		++num;
	return num;
}

void TikzEditor::showCursorPosition()
{
	QTextCursor cursor = textCursor();
	const int cursorPosition = cursor.position();
	cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
	const int startPosition = cursor.position();
	emit cursorPositionChanged(cursor.blockNumber() + 1, cursorPosition - startPosition + 1);
}

/**
 * Determines the word on which the cursor is positioned
 * (necessary for completion).
 */

QString TikzEditor::textUnderCursor() const
{
	QTextCursor cursor = textCursor();
	const int oldPos = cursor.position();
//	cursor.select(QTextCursor::WordUnderCursor);
//	const int newPos = cursor.selectionStart();
	int newPos;
	for (newPos = oldPos; newPos > 0;) // move the cursor to the beginning of the word
	{
		cursor.setPosition(--newPos, QTextCursor::KeepAnchor);
		if (cursor.selectedText().trimmed().isEmpty()) // if the current char is a whitespace, then we have reached the beginning of the word
		{
			cursor.clearSelection();
			cursor.setPosition(++newPos, QTextCursor::MoveAnchor);
			break;
		}
		else if (cursor.selectedText() == "\\" || cursor.atBlockStart()) // these characters also delimit the beginning of the word (the beginning of a TikZ command)
		{
			cursor.clearSelection();
			break;
		}
		else if (cursor.selectedText() == "["
		    || cursor.selectedText() == ",") // these characters also delimit the beginning of the word (the beginning of a TikZ option)
		{
			cursor.clearSelection();
			cursor.setPosition(++newPos, QTextCursor::MoveAnchor);
			break;
		}
		cursor.clearSelection();
	}
//	cursor.setPosition(newPos, QTextCursor::MoveAnchor);
	cursor.setPosition(oldPos, QTextCursor::KeepAnchor);
	QString word = cursor.selectedText();
//	if (word.right(1) != word.trimmed().right(1))
//		word = "";
	return word;
}

void TikzEditor::keyPressEvent(QKeyEvent *event)
{
	const Qt::KeyboardModifiers modifier = QApplication::keyboardModifiers();

	/* completer */
	if (m_completer && m_completer->popup()->isVisible())
	{
		// the following keys are forwarded by the completer to the widget
		switch (event->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				event->ignore();
				return;
		}
	}

	/* scroll viewport when Ctrl+Up and Ctrl+Down are pressed */
	if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Up)
	{
		const int dy = -1 + verticalScrollBar()->value();
		verticalScrollBar()->setValue(dy);
	}
	else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Down)
	{
		const int dy = 1 + verticalScrollBar()->value();
		verticalScrollBar()->setValue(dy);
	}
	/* ensure that PageUp and PageDown keep the cursor at the same visible place */
/*
	else if (event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown)
	{
		const QTextCursor::MoveOperation moveOperation
		    = (event->key() == Qt::Key_PageUp) ? QTextCursor::Up : QTextCursor::Down;
		QTextCursor cursor = textCursor();
		const QFontMetrics fontMetrics(document()->defaultFont());
		const int repeat = viewport()->height() / fontMetrics.lineSpacing() - 1;
		const int oldPosition = cursorRect().top();
		if (modifier & Qt::ShiftModifier)
			cursor.movePosition(moveOperation, QTextCursor::KeepAnchor, repeat);
		else
			cursor.movePosition(moveOperation, QTextCursor::MoveAnchor, repeat);
		setTextCursor(cursor);
		const int newPosition = verticalScrollBar()->value() + cursorRect().top() - oldPosition;
		verticalScrollBar()->setValue(newPosition);
		ensureCursorVisible();
	}
*/
	/* the first time End is pressed moves the cursor to the end of the line, the second time to the end of the block */
	else if (event->key() == Qt::Key_Home
	    && !(modifier & Qt::ControlModifier)
	    && !(modifier & Qt::ShiftModifier))
	{
		QTextCursor cursor = textCursor();
		const int oldPosition = cursor.position();
		cursor.movePosition(QTextCursor::StartOfLine);
		if (cursor.position() == oldPosition)
			cursor.movePosition(QTextCursor::StartOfBlock);
		setTextCursor(cursor);
		ensureCursorVisible();
	}
	else if (event->key() == Qt::Key_End
	    && !(modifier & Qt::ControlModifier)
	    && !(modifier & Qt::ShiftModifier))
	{
		QTextCursor cursor = textCursor();
		const int oldPosition = cursor.position();
		cursor.movePosition(QTextCursor::EndOfLine);
		if (cursor.position() == oldPosition)
			cursor.movePosition(QTextCursor::EndOfBlock);
		setTextCursor(cursor);
		ensureCursorVisible();
	}
	/* keys that change the content without moving the cursor may alter the brackets too */
	else if (event->key() == Qt::Key_Delete || (event->key() == Qt::Key_Z && (modifier & Qt::ControlModifier)))
	{
		QPlainTextEdit::keyPressEvent(event);
		highlightCurrentLine();
		matchBrackets(); // calculate new bracket highlighting
	}
	/* go to next argument in text inserted with code completion */
	else if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab)
	{
		QTextCursor cursor = textCursor();
		QTextBlock block = cursor.block();
		QTextDocument::FindFlags flags = 0;
		if (event->key() == Qt::Key_Backtab)
			flags = QTextDocument::FindBackward;
		if (block.isValid() && block.text().contains(s_completionPlaceHolder))
		{
			cursor = document()->find(s_completionPlaceHolder, cursor, flags);
			if (!cursor.isNull())
				setTextCursor(cursor);
			else
				QPlainTextEdit::keyPressEvent(event);
		}
		else
			QPlainTextEdit::keyPressEvent(event);
	}
	else
		QPlainTextEdit::keyPressEvent(event);

	/* completer */
	if (m_completer)
	{
//		const QString endOfWord("~!@#$%^&*()_+{}|:\"<>?,./;'[]-= ");
		const QString completionPrefix = textUnderCursor();
		if ((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
		    || (event->text().isEmpty() && event->key() != Qt::Key_AltGr)
		    || completionPrefix.length() < 3)
//		    || endOfWord.contains(event->text().right(1)))
		{
			m_completer->popup()->hide();
		}
		else
		{
			if (completionPrefix != m_completer->completionPrefix())
			{
				m_completer->setCompletionPrefix(completionPrefix);
				m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
			}
			if (m_completer->completionPrefix() != m_completer->currentCompletion())
//			    || m_completer->completionCount() > 1)
			{
				QTextCursor cursor = textCursor();
//				cursor.movePosition(QTextCursor::StartOfWord);
				cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, m_completer->completionPrefix().length());
				QRect rect = cursorRect(cursor);
				rect.translate(5, 5);
				rect.setWidth(m_completer->popup()->sizeHintForColumn(0)
				    + m_completer->popup()->verticalScrollBar()->sizeHint().width());
				m_completer->complete(rect); // show popup
			}
			else
				m_completer->popup()->hide();
		}
	}
}

void TikzEditor::focusInEvent(QFocusEvent *event)
{
	if (m_completer)
		m_completer->setWidget(this);

	emit focusIn();

	QPlainTextEdit::focusInEvent(event);
}

void TikzEditor::focusOutEvent(QFocusEvent *event)
{
	emit focusOut();
	QPlainTextEdit::focusOutEvent(event);
}

void TikzEditor::setCompleter(QCompleter *completer)
{
	if (m_completer)
		disconnect(m_completer, 0, this, 0);

	m_completer = completer;
	if (!m_completer)
		return;

	m_completer->setWidget(this);
	m_completer->setCompletionMode(QCompleter::PopupCompletion);
	m_completer->setCaseSensitivity(Qt::CaseInsensitive);
	connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void TikzEditor::insertCompletion(const QString &completion)
{
	if (!m_completer || m_completer->widget() != this)
		return;

	emit showStatusMessage(completion, 0);

	QTextCursor cursor = textCursor();
	const int extra = completion.length() - m_completer->completionPrefix().length();
	const int pos = cursor.position();

	// remove all options (between <...>) and put cursor at the first option
	QString insertWord = completion.right(extra);
	const QRegExp rx("<[^<>]*>");
	const int offset = rx.indexIn(insertWord) - 1; // put cursor at the first option
	insertWord.replace(rx, s_completionPlaceHolder);

	cursor.insertText(insertWord);
	if (insertWord.contains(s_completionPlaceHolder))
	{
		cursor.setPosition(pos, QTextCursor::MoveAnchor);
		cursor = document()->find(s_completionPlaceHolder, cursor);
	}
	else if (offset > 0)
		cursor.setPosition(pos + offset + 1, QTextCursor::MoveAnchor);
	setTextCursor(cursor);
}
