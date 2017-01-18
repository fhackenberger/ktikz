/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014                *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
 *   Copyright (C) 2013 by João Carreira <jfmcarreira@gmail.com>           *
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

#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QTextBlock>
#include <QtGui/QTextLayout>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QScrollBar>
#include <QtCore/QStringListModel>
#else
#include <QtGui/QAbstractItemView>
#include <QtGui/QApplication>
#include <QtGui/QCompleter>
#include <QtGui/QScrollBar>
#include <QtGui/QStringListModel>
#endif

#include "linenumberwidget.h"

static const QString s_completionPlaceHolder(0x2022);

TikzEditor::TikzEditor(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_highlightCurrentLineColor(Qt::yellow)
	, m_highlightCurrentLine(true)
	, m_oldVerticalScrollBarValue(0)
	, m_whiteSpacesColor(Qt::gray)
	, m_tabulatorsColor(Qt::gray)
	, m_matchingColor(Qt::darkGreen)
	, m_showWhiteSpaces(true)
	, m_showTabulators(true)
	, m_showMatchingBrackets(true)
	, m_completer(0)
	, m_oldNumOfLines(0)
	, m_showLineNumberArea(true)
{
	m_lineNumberArea = new LineNumberWidget(this);
	updateLineNumberAreaWidth();

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	highlightCurrentLine();

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(showCursorPosition()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchBrackets()));
	connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(recalculateBookmarks(int)));

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
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

/***************************************************************************/

namespace {

bool isBracket(const QChar &c)
{
	return c == QLatin1Char('(') || c == QLatin1Char('{') || c == QLatin1Char('[')
		|| c == QLatin1Char(')') || c == QLatin1Char('}') || c == QLatin1Char(']');
}

} // anonymous namespace

void TikzEditor::matchBrackets()
{
	// clear previous bracket highlighting
	if (!isReadOnly())
	{
		QList<QTextEdit::ExtraSelection> extraSelections;
		QTextEdit::ExtraSelection selection;
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
		setExtraSelections(extraSelections);
	}

	if (!m_showMatchingBrackets)
		return;

	// find current matching brackets
	const QTextCursor cursor = textCursor();
	const QTextDocument *document = this->document();
	int pos = cursor.position();
	if (!isBracket(document->characterAt(pos)) && (pos <= 0 || !isBracket(document->characterAt(--pos)))) // we do --pos because we want to match the bracket at both sides of the cursor
		return;

	// get corresponding opening/closing bracket and search direction
	const QChar car = document->characterAt(pos);
	QChar matchCar;
	int inc = 1;
	if (car == QLatin1Char('(')) matchCar = QLatin1Char(')');
	else if (car == QLatin1Char('{')) matchCar = QLatin1Char('}');
	else if (car == QLatin1Char('[')) matchCar = QLatin1Char(']');
	else
	{
		inc = -1;
		if (car == QLatin1Char(')')) matchCar = QLatin1Char('(');
		else if (car == QLatin1Char('}')) matchCar = QLatin1Char('{');
		else if (car == QLatin1Char(']')) matchCar = QLatin1Char('[');
		else
			return;
	}

	// find location of the corresponding bracket
	int matchingBegin = pos;
	int matchingEnd = -1;
	int numOfMatchCharsToSkip = 0;
	for (; pos >= 0 && pos < document->characterCount(); pos += inc)
	{
		if (document->characterAt(pos) == car) // if the brackets are nested, then don't match the closing bracket of the nested open bracket, e.g. in (()), don't match the first ) with the first (
			numOfMatchCharsToSkip++;
		else if (document->characterAt(pos) == matchCar)
		{
			numOfMatchCharsToSkip--;
			if (numOfMatchCharsToSkip == 0)
			{
				matchingEnd = pos;
				break;
			}
		}
	}

	if (matchingBegin > matchingEnd)
		qSwap(matchingBegin, matchingEnd);

	// if there is a match, then show it
	if (matchingBegin != -1)
		showMatchingBrackets(matchingBegin, matchingEnd);
}

void TikzEditor::showMatchingBrackets(int matchingBegin, int matchingEnd)
{
	if (isReadOnly())
		return;

	for (QTextBlock block = firstVisibleBlock(); block.isValid(); block = block.next())
	{
		if (blockBoundingGeometry(block).top() > viewport()->height())
			break;

		const int textLength = block.text().length();
		const int blockPosition = block.position();
		for (int i = 0; i < textLength; ++i)
		{
			if (blockPosition + i == matchingBegin || blockPosition + i == matchingEnd)
			{
				QList<QTextEdit::ExtraSelection> extraSelectionList = extraSelections();
				QTextEdit::ExtraSelection selection;
				selection.format.setBackground(m_matchingColor);
				selection.cursor = textCursor();
				selection.cursor.setPosition(blockPosition + i, QTextCursor::MoveAnchor);
				selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
				extraSelectionList.append(selection);
				setExtraSelections(extraSelectionList);
			}
		}
	}
}

/***************************************************************************/

void TikzEditor::setShowWhiteSpaces(bool visible)
{
	m_showWhiteSpaces = visible;
}

void TikzEditor::setShowTabulators(bool visible)
{
	m_showTabulators = visible;
}

void TikzEditor::setShowMatchingBrackets(bool visible)
{
	m_showMatchingBrackets = visible;
}

void TikzEditor::setHighlightCurrentLine(bool visible)
{
	m_highlightCurrentLine = visible;
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

void TikzEditor::setHighlightCurrentLineColor(const QColor &color)
{
	m_highlightCurrentLineColor = color;
}

void TikzEditor::paintTabstop(QPainter &painter, qreal x, qreal y, int spaceWidth)
{
	qreal dist = spaceWidth * 0.3;
/*
	QPointF points[8];
	points[0] = QPointF(x - dist, y - dist);
	points[1] = QPointF(x, y);
	points[2] = QPointF(x, y);
	points[3] = QPointF(x - dist, y + dist);
	x += spaceWidth / 3.0;
	points[4] = QPointF(x - dist, y - dist);
	points[5] = QPointF(x, y);
	points[6] = QPointF(x, y);
	points[7] = QPointF(x - dist, y + dist);
	painter.drawLines(points, 4);
*/
	QPainterPath tabStopPath;
	tabStopPath.moveTo(x - dist, y - dist);
	tabStopPath.lineTo(x, y);
	tabStopPath.lineTo(x - dist, y + dist);
	x += spaceWidth / 3.0;
	tabStopPath.moveTo(x - dist, y - dist);
	tabStopPath.lineTo(x, y);
	tabStopPath.lineTo(x - dist, y + dist);
	painter.drawPath(tabStopPath);
}

void TikzEditor::paintSpace(QPainter &painter, qreal x, qreal y, int spaceWidth)
{
	Q_UNUSED(spaceWidth);
	painter.drawPoint(QPointF(x, y));
}

void TikzEditor::printWhiteSpaces(QPainter &painter)
{
	const QFontMetrics fontMetrics = QFontMetrics(document()->defaultFont());
	const int spaceWidth = fontMetrics.width(QLatin1Char(' '));
	const int fontHeight = fontMetrics.height();
	QTextCursor cursor = textCursor();

	QPen backupPen(painter.pen());
	QPen tabulatorsPen(m_tabulatorsColor);
	tabulatorsPen.setWidthF(qMax(qreal(0.5), qreal(spaceWidth * .1)));
	tabulatorsPen.setCapStyle(Qt::RoundCap);
	tabulatorsPen.setJoinStyle(Qt::RoundJoin);
	QPen whiteSpacesPen(m_whiteSpacesColor);
	whiteSpacesPen.setWidthF(spaceWidth / 3.5);
	whiteSpacesPen.setCapStyle(Qt::RoundCap);

	for (QTextBlock block = firstVisibleBlock(); block.isValid(); block = block.next())
	{
		if (blockBoundingGeometry(block).top() > viewport()->height()) // only paint white spaces in the visible part of the text
			break;

		const QString text = block.text();
		const int textLength = text.length();
		const int blockPosition = block.position();
		for (int i = 0; i < textLength; ++i)
		{
			cursor.setPosition(blockPosition + i, QTextCursor::MoveAnchor);
			const QRect rect = cursorRect(cursor);

//			const QFontMetrics fontMetrics = QFontMetrics(cursor.charFormat().font());

			if (m_showWhiteSpaces && text.at(i) == QLatin1Char(' '))
			{
				if (painter.pen() != whiteSpacesPen)
					painter.setPen(whiteSpacesPen);
				paintSpace(painter, rect.x() + spaceWidth / 2.0, rect.y() + fontHeight / 2.0, spaceWidth);
			}
			else if (m_showTabulators && text.at(i) == QLatin1Char('\t'))
			{
				if (painter.pen() != tabulatorsPen)
					painter.setPen(tabulatorsPen);
				paintTabstop(painter, rect.x() + spaceWidth / 2.0, rect.y() + fontHeight / 2.0, spaceWidth);
			}
		}
	}
	painter.setPen(backupPen);
}

/***************************************************************************/

void TikzEditor::paintEvent(QPaintEvent *event)
{
	QPainter painter(viewport());

	// highlight current line
	if (m_highlightCurrentLine)
	{
		QRect rect = cursorRect();
		rect.setX(0);
		rect.setWidth(viewport()->width());
		painter.fillRect(rect, QBrush(m_highlightCurrentLineColor));
	}

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

void TikzEditor::setCursorPosition(int row, int col)
{
	const int pos = document()->findBlockByNumber(row).position() + col;
	QTextCursor cursor = textCursor();
	cursor.setPosition(pos, QTextCursor::MoveAnchor);
	setTextCursor(cursor);

	// make sure that the cursor is in the middle of the visible area
	centerCursor();
	setFocus();
}

int TikzEditor::numOfLines() const
{
	return document()->blockCount();
}

void TikzEditor::showCursorPosition()
{
	QTextCursor cursor = textCursor();
	Q_EMIT cursorPositionChanged(cursor.blockNumber() + 1, cursor.position() - cursor.block().position() + 1);
}

/**
 * Determines the word on which the cursor is positioned
 * (necessary for completion).
 */

QString TikzEditor::textUnderCursor() const
{
	QTextCursor cursor = textCursor();
	QTextDocument *document = this->document();
	const int startOfLine = cursor.block().position();
	int position;
	for (position = cursor.position() - 1; position > 0; --position) // find the beginning of the word
	{
		const QChar character = document->characterAt(position);
		if (character.isSpace() // if the current char is a whitespace, then we have reached the beginning of the word
		    || character == QLatin1Char('[') || character == QLatin1Char(',')) // these characters also delimit the beginning of the word (the beginning of a TikZ option)
		{
			++position;
			break;
		}
		else if (character == QLatin1Char('\\') || position == startOfLine) // these characters also delimit the beginning of the word (the beginning of a TikZ command)
		{
			break;
		}
	}
	if (position < 0)
		position = 0;
	cursor.setPosition(position, QTextCursor::KeepAnchor);
	return cursor.selectedText();
}

void TikzEditor::keyPressEvent(QKeyEvent *event)
{
	const Qt::KeyboardModifiers modifier = QApplication::keyboardModifiers();

	// completer
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

	// scroll viewport when Ctrl+Up and Ctrl+Down are pressed
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
	// the first time End is pressed moves the cursor to the end of the line, the second time to the end of the block
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
	// keys that change the content without moving the cursor may alter the brackets too
	else if (event->key() == Qt::Key_Delete || (event->key() == Qt::Key_Z && (modifier & Qt::ControlModifier)))
	{
		QPlainTextEdit::keyPressEvent(event);
		highlightCurrentLine();
		matchBrackets(); // calculate new bracket highlighting
	}
	// go to next argument in text inserted with code completion
	else if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab)
	{
		QTextCursor cursor = textCursor();
		QTextBlock block = cursor.block();
		QTextDocument::FindFlags flags = 0;
		if (event->key() == Qt::Key_Backtab)
			flags = QTextDocument::FindBackward;
		if (cursor.hasSelection() && cursor.selectedText().contains(QChar::ParagraphSeparator))
		{
			Q_EMIT tabIndent(event->key() == Qt::Key_Backtab);
			return;
		}
		else if (block.isValid() && block.text().contains(s_completionPlaceHolder))
		{
			cursor = document()->find(s_completionPlaceHolder, cursor, flags);
			if (!cursor.isNull())
			{
				setTextCursor(cursor);
				return;
			}
		}
		// the following is done when there is no multiline selection and no s_completionPlaceHolder can be found
		cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
		QString selectedText = cursor.selectedText();
		if (selectedText.remove(QLatin1Char(' ')).remove(QLatin1Char('\t')).isEmpty() || event->key() == Qt::Key_Backtab)
			Q_EMIT tabIndent(event->key() == Qt::Key_Backtab);
		else
			QPlainTextEdit::keyPressEvent(event);
		return;
	}
	else
		QPlainTextEdit::keyPressEvent(event);

	// completer
	if (m_completer)
	{
//		const QString endOfWord(QLatin1String("~!@#$%^&*()_+{}|:\"<>?,./;'[]-= "));
		const QString completionPrefix = textUnderCursor();
		if ((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
		        || (event->text().isEmpty() && event->key() != Qt::Key_AltGr)
		        || completionPrefix.length() < 3)
//		        || endOfWord.contains(event->text().right(1)))
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
				rect.translate(5 + m_lineNumberArea->width(), 5); // reposition popup
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

	Q_EMIT focusIn();

	QPlainTextEdit::focusInEvent(event);
}

void TikzEditor::focusOutEvent(QFocusEvent *event)
{
	Q_EMIT focusOut();
	QPlainTextEdit::focusOutEvent(event);
}

/***************************************************************************/

void TikzEditor::updateCompleter(bool useCompletion, const QStringList &words)
{
	if (!useCompletion)
	{
		if (m_completer)
			disconnect(m_completer, 0, this, 0);
		delete m_completer;
		m_completer = 0;
		return;
	}

	if (!m_completer)
	{
		m_completer = new QCompleter(this);
		m_completer->setWidget(this);
		m_completer->setCompletionMode(QCompleter::PopupCompletion);
		m_completer->setCaseSensitivity(Qt::CaseInsensitive);
		m_completer->setWrapAround(false);
		connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
	}

	QStringListModel *model = new QStringListModel(words, m_completer);
	m_completer->setModel(model);
	m_completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
}

void TikzEditor::insertCompletion(const QString &completion)
{
	if (!m_completer || m_completer->widget() != this)
		return;

	Q_EMIT showStatusMessage(completion, 0);

	QTextCursor cursor = textCursor();
	const int extra = completion.length() - m_completer->completionPrefix().length();
	const int pos = cursor.position();

	// remove all options (between <...>) and put cursor at the first option
	QString insertWord = completion.right(extra);
	const QRegExp rx(QLatin1String("<[^<>]*>"));
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

/***************************************************************************/

/*!
 * This function inserts lineNumber in the list of bookmarks in such a way
 * that the list is sorted from small to large.  If lineNumber is already
 * in the list, then it is removed from the list.
 */
void TikzEditor::toggleUserBookmark(int lineNumber)
{
	if (lineNumber <= 0 || lineNumber > document()->blockCount())
		return;
	for (int i = 0; i < m_userBookmarks.length(); ++i)
	{
		if (m_userBookmarks.at(i) == lineNumber)
		{
			m_userBookmarks.removeAt(i);
			m_lineNumberArea->update();
			return;
		}
		else if (m_userBookmarks.at(i) > lineNumber)
		{
			m_userBookmarks.insert(i, lineNumber);
			m_lineNumberArea->update();
			return;
		}
	}
	m_userBookmarks.append(lineNumber); // if lineNumber is larger than any number already in the list, then we insert lineNumber at the end of the list
	m_lineNumberArea->update();
}

void TikzEditor::toggleUserBookmark()
{
	const int lineNumber = textCursor().blockNumber() + 1;
	toggleUserBookmark(lineNumber);
}

/*!
 * This functions returns the line number of the bookmark with index
 * 'which' if 'which' is a valid index, and returns -1 otherwise.
 */

int TikzEditor::userBookmark(int which) const
{
	return which >= 0 && which < m_userBookmarks.length() ? m_userBookmarks.at(which) : -1;
}

void TikzEditor::previousUserBookmark()
{
	const int lineNumber = textCursor().blockNumber() + 1;
	for (int i = m_userBookmarks.length() - 1; i >= 0; --i)
	{
		if (lineNumber > m_userBookmarks.at(i))
		{
			goToLine(m_userBookmarks.at(i) - 1);
			return;
		}
	}
}

void TikzEditor::nextUserBookmark()
{
	const int lineNumber = textCursor().blockNumber() + 1;
	for (int i = 0; i < m_userBookmarks.length(); ++i)
	{
		if (lineNumber < m_userBookmarks.at(i))
		{
			goToLine(m_userBookmarks.at(i) - 1);
			return;
		}
	}
}

/*!
 * When lines are added or removed in the editor, this functions allows
 * to recalculate the line numbers of the bookmarks and remove the bookmarks
 * which were on removed lines. The line numbers of the remaining bookmarks
 * are recalculated so that they still point to the correct text.
 */

void TikzEditor::recalculateBookmarks(int position)
{
	const int addedLines = numOfLines() - m_oldNumOfLines;
	const int lineNumber = document()->findBlock(position).blockNumber() + 1;
	if (addedLines != 0)
	{
		for (int i = 0; i < m_userBookmarks.length(); ++i)
		{
			// if a line containing a bookmark is removed, then the bookmark itself must also be removed
			if (addedLines < 0 && lineNumber <= m_userBookmarks.at(i)
			    && lineNumber - addedLines > m_userBookmarks.at(i))
			{
				m_userBookmarks.removeAt(i);
				--i;
			}
			else if (lineNumber <= m_userBookmarks.at(i)) // shift all bookmarks that come after the insertion or deletion of lines
				m_userBookmarks[i] += addedLines;
		}
	}
	m_oldNumOfLines = numOfLines();
}

QList<int> TikzEditor::userBookmarks() const
{
	return m_userBookmarks;
}

void TikzEditor::setUserBookmarks(const QList<int> &bookmarks)
{
	m_userBookmarks.clear();
	for (int i = 0; i < bookmarks.length(); ++i)
	{
		if (bookmarks.at(i) > 0 && bookmarks.at(i) <= numOfLines())
			toggleUserBookmark(bookmarks.at(i));
	}
}

/***************************************************************************/

int TikzEditor::lineNumberAreaWidth()
{
	int digits = 1;
	for (int max = qMax(1, blockCount()); max >= 10; max /= 10)
		++digits;
	digits = qMax(4, digits) + 1;

	return m_showLineNumberArea ? 3 + fontMetrics().width(QLatin1Char('9')) * digits : 0;
}

void TikzEditor::updateLineNumberAreaWidth()
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TikzEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		m_lineNumberArea->scroll(0, dy);
	else
		m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth();
}

void TikzEditor::resizeEvent(QResizeEvent *event)
{
	QPlainTextEdit::resizeEvent(event);

	QRect rect = contentsRect();
	m_lineNumberArea->setGeometry(QRect(rect.left(), rect.top(), lineNumberAreaWidth(), rect.height()));
}

void TikzEditor::setShowLineNumberArea(bool visible)
{
	m_showLineNumberArea = visible;
	m_lineNumberArea->setVisible(visible);
	updateLineNumberAreaWidth();
}
