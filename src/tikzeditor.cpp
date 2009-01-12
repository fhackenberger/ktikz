/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
 *   Glad.Deschrijver@UGent.be                                             *
 *                                                                         *
 *   Bracket matching and white space showing code originally from         *
 *   qdevelop: (C) 2006 Jean-Luc Biord (http://qdevelop.org)               *
 *   Completion code originally from texmaker:                             *
 *   (C) 2003-2008 Pascal Brachet (http://www.xm1math.net/texmaker)        *
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

#include <QAbstractItemView>
#include <QApplication>
#include <QCompleter>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextLayout>

#include "tikzeditor.h"

static const char *const tabPixmap_img[] =
{
	/* width height ncolors cpp [x_hot y_hot] */
	"8 8 3 2 0 0",
	/* colors */
	"  s none       m none  c none",
	"O s iconColor1 m black c black",
	"X s iconColor2 m black c #B0B0B0",
	/* pixels */
	"  X     X       ",
	"    X     X     ",
	"      X     X   ",
	"        X     X ",
	"      X     X   ",
	"    X     X     ",
	"  X     X       ",
	"                ",
};

static const char *const spacePixmap_img[] =
{
	/* width height ncolors cpp [x_hot y_hot] */
	"8 8 3 2 0 0",
	/* colors */
	"  s none       m none  c none",
	"O s iconColor1 m black c black",
	"X s iconColor2 m black c #B0B0B0",
	/* pixels */
	"                ",
	"                ",
 	"                ",
	"                ",
	"                ",
	"      X         ",
	"      X X       ",
	"                ",
};

TikzEditor::TikzEditor(QWidget *parent) : QTextEdit(parent)
{
	m_tabPixmap = QPixmap(tabPixmap_img);
	m_spacePixmap = QPixmap(spacePixmap_img);
	m_showWhiteSpaces = true;
	m_showMatchingBrackets = true;
	m_matchingColor = Qt::yellow;

	m_completer = 0;

	setAcceptRichText(false);
//	setLineWidth(0);
//	setFrameShape(QFrame::NoFrame);

	connect(this, SIGNAL(cursorPositionChanged()), viewport(), SLOT(update()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(showCursorPosition()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchBrackets()));
}

TikzEditor::~TikzEditor()
{
}

void TikzEditor::matchBrackets()
{
	m_matchingBegin = -1;
	m_matchingEnd = -1;
	if (!m_showMatchingBrackets) return;

	m_plainText = toPlainText();
// 	QString matchText = simplifiedText(plainText);
	const QString matchText = m_plainText;
	const QTextCursor cursor = textCursor();
	int pos = cursor.position();
//	if (pos == -1 || cursor.atEnd() || !QString("({[]})").contains(m_plainText.at(pos)))
//		return;
	if (pos == -1)
		return;
	else if (cursor.atEnd() || !QString("({[]})").contains(m_plainText.at(pos)))
	{
		if (pos <= 0 || !QString("({[]})").contains(m_plainText.at(--pos)))
			return;
	}

	QChar car;
	if (pos != -1)
	{
		if (!cursor.atEnd())
			car = matchText.at(pos);
		else
			car = matchText.at(pos - 1);
	}
	QChar matchCar;
	long inc = 1;
	if (car == '(') matchCar = ')';
	else if (car == '{') matchCar = '}';
	else if (car == '[') matchCar = ']';
	else if (car == ')')
	{
		matchCar = '(';
		inc = -1;
	}
	else if (car == '}')
	{
		matchCar = '{';
		inc = -1;
	}
	else if(car == ']')
	{
		matchCar = '[';
		inc = -1;
	}
	else
		return;

	m_matchingBegin = pos;
	int nb = 0;
	do
	{
		if (matchText.at(pos) == car)
			nb++;
		else if (matchText.at(pos) == matchCar)
		{
			nb--;
			if(nb == 0)
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
}

void TikzEditor::setShowMatchingBrackets(bool showMatchingBrackets)
{
	m_showMatchingBrackets = showMatchingBrackets;
}

void TikzEditor::setMatchingColor(const QColor &matchingColor)
{
	m_matchingColor = matchingColor;
}

void TikzEditor::setShowWhiteSpaces(bool showWhiteSpaces)
{
	m_showWhiteSpaces = showWhiteSpaces;
}

void TikzEditor::printWhiteSpaces(QPainter &painter)
{
	const int contentsY = verticalScrollBar()->value();
	const qreal pageBottom = contentsY + viewport()->height();
	const QFontMetrics fontMetrics = QFontMetrics(document()->defaultFont());

	for (QTextBlock block = document()->begin(); block.isValid(); block = block.next())
	{
		QTextLayout *layout = block.layout();
		const QRectF boundingRect = layout->boundingRect();
		const QPointF position = layout->position();

		if (position.y() + boundingRect.height() < contentsY)
			continue;
		if (position.y() > pageBottom)
			break;

		const QString text = block.text();
		const int textLength = text.length();

		for (int i = 0; i < textLength; ++i)
		{
			QTextCursor cursor = textCursor();
			cursor.setPosition(block.position() + i, QTextCursor::MoveAnchor);
			const QRect rect = cursorRect(cursor);

			if (block.position() + i == m_matchingBegin || block.position() + i == m_matchingEnd)
			{
				QTextCursor cursor2 = textCursor();
				cursor2.setPosition(block.position() + i, QTextCursor::MoveAnchor);
				QRect rect2 = cursorRect(cursor2);
				if (QString("({[").contains(m_plainText.at(block.position() + i)))
					rect2.adjust(rect2.width() / 3, rect2.height() - 1, rect2.width() / 3, 0);
				else
					rect2.adjust(rect2.width() / 4, rect2.height() - 1, rect2.width() / 4, 0);
				const int charWidth = fontMetrics.width(m_plainText.at(block.position() + i));
				// Underline brackets
				//rect2.adjust(0, 0, charWidth, 0);
				//painter.setPen(m_matchingColor);
				//painter.drawRect(rect2);
				// Fill brackets bounding box
				rect2.adjust(0, -fontMetrics.height(), charWidth, 0);
				painter.fillRect(rect2, QBrush(m_matchingColor));
			}

			if (m_showWhiteSpaces)
			{
				QPixmap *pixmap = 0;
				if (text.at(i) == ' ')
					pixmap = &m_spacePixmap;
				else if (text.at(i) == '\t')
					pixmap = &m_tabPixmap;
				else
					continue;
				painter.drawPixmap(rect.x(), rect.y() + fontMetrics.height() / 2 - 5, *pixmap);
			}
		}
	}
}

void TikzEditor::paintEvent(QPaintEvent *event)
{
	const QColor lineColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Base));
	const QColor altLineColor(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::AlternateBase));
	QBrush brush;
	if (lineColor == altLineColor)
		brush = QBrush(lineColor.darker(105));
	else
		brush = QBrush(altLineColor);

	QRect rect = cursorRect();
	rect.setX(0);
	rect.setWidth(viewport()->width());
	QPainter painter(viewport());
	painter.fillRect(rect, brush);
	painter.end();

	QPainter painter2(viewport());
	if (m_showWhiteSpaces || m_matchingBegin != -1)
		printWhiteSpaces(painter2);
	painter2.end();

	QTextEdit::paintEvent(event);
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
	ensureCursorVisible();
	const int newPosition = verticalScrollBar()->value() + cursorRect().top() - viewport()->height() / 2;
	verticalScrollBar()->setValue(newPosition);
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

QString TikzEditor::textUnderCursor() const
{
	QTextCursor cursor = textCursor();
	const int oldPos = cursor.position();
	cursor.select(QTextCursor::WordUnderCursor);
	const int newPos = cursor.selectionStart();
	cursor.setPosition(newPos, QTextCursor::MoveAnchor);
	cursor.setPosition(oldPos, QTextCursor::KeepAnchor);
	QString word = cursor.selectedText();
	if (word.right(1) != word.trimmed().right(1))
		word = "";
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
	if (event->key() == Qt::Key_Up && event->modifiers() == Qt::ControlModifier)
	{
		const QRect rect = cursorRect();
		const int dy = -rect.height() + verticalScrollBar()->value();
		verticalScrollBar()->setValue(dy);
	}
	else if (event->key() == Qt::Key_Down && event->modifiers() == Qt::ControlModifier)
	{
		const QRect rect = cursorRect();
		const int dy = rect.height() + verticalScrollBar()->value();
		verticalScrollBar()->setValue(dy);
	}
	/* ensure that PageUp and PageDown keep the cursor at the same visible place */
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
		QTextEdit::keyPressEvent(event);
		matchBrackets(); // calculate new bracket highlighting
	}
	else
		QTextEdit::keyPressEvent(event);

	/* completer */
	if (m_completer)
	{
//		const QString endOfWord("~!@#$%^&*()_+{}|:\"<>?,./;'[]-= ");
		const QString completionPrefix = textUnderCursor();
		if ((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
		    || event->text().isEmpty() || completionPrefix.length() < 3)
//	    	|| endOfWord.contains(event->text().right(1)))
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
				QRect rect = cursorRect();
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
	QTextEdit::focusInEvent(event);
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
	connect(m_completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

void TikzEditor::insertCompletion(const QString &completion)
{
	if (!m_completer || m_completer->widget() != this)
		return;

	QTextCursor cursor = textCursor();
	const int extra = completion.length() - m_completer->completionPrefix().length();
	const int pos = cursor.position();

	// remove all options (between <...>) and put cursor at the first option
	QString insertWord = completion.right(extra);
	const QRegExp rx("<[^<>]*>");
	const int offset = rx.indexIn(insertWord) - 1; // put cursor at the first option
	insertWord.remove(rx);

	cursor.insertText(insertWord);
	if (offset > 0)
		cursor.setPosition(pos + offset + 1, QTextCursor::MoveAnchor);
	setTextCursor(cursor);
}
