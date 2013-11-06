/***************************************************************************
 *   Copyright (C) 2007, 2008, 2011, 2012, 2013 by Glad Deschrijver        *
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

#include "linenumberwidget.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>

#include "tikzeditor.h"

LineNumberWidget::LineNumberWidget(TikzEditor *editor)
    : QWidget(editor), m_editor(editor)
{
	setAutoFillBackground(true);

	QPalette p(palette());
	const QPalette standardPalette(p); // const QPalette standardPalette(QApplication::style()->standardPalette()); is slow, since we are in the constructor palette() == QApplication::palette()
	const QColor windowColor(standardPalette.color(QPalette::Normal, QPalette::Window));
	p.setColor(backgroundRole(), windowColor.lighter(102));
	setPalette(p);

	m_highlightColor = QColor(standardPalette.color(QPalette::Normal, QPalette::Highlight));
	m_highlightBrush = QBrush(m_highlightColor);
	m_highlightPen = QPen(m_highlightColor);
	m_highlightPen.setStyle(Qt::SolidLine);
	m_highlightedTextPen = QPen(standardPalette.color(QPalette::Normal, QPalette::HighlightedText));

	setToolTip(tr("Click to add or remove a bookmark"));
}

QSize LineNumberWidget::sizeHint() const
{
	return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void LineNumberWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	const QFontMetrics fm = m_editor->fontMetrics();

	// draw separator between line number area and text editor
	painter.setPen(m_highlightPen);
	painter.drawLine(width() - 2, 0, width() - 2, height());

	QTextBlock block = m_editor->firstVisibleBlock();
	int lineNumber = block.blockNumber() + 1;
	int top = (int) m_editor->blockBoundingGeometry(block).translated(m_editor->contentOffset()).top();
	int lineHeight = m_editor->blockBoundingRect(block).height();
	int bottom = top + lineHeight;

	QList<int> userBookmarks = m_editor->userBookmarks();
	while (block.isValid() && top <= event->rect().bottom())
	{
		if (bottom >= event->rect().top())
		{
			for (int i = 0; i < userBookmarks.length(); ++i)
			{
				if (userBookmarks.at(i) == lineNumber)
				{
					painter.fillRect(2, top, fm.width(QLatin1Char('B')) + 4, lineHeight, m_highlightBrush);
					painter.setPen(m_highlightedTextPen);
					painter.drawText(4, top, width() - 4, lineHeight, Qt::AlignLeft | Qt::AlignTop, QLatin1String("B"));
					painter.setPen(m_highlightPen);
//					update(0, top, width(), lineHeight); // make sure the bookmark is visible even when the line is wrapped
					break;
				}
			}
			painter.drawText(0, top, width() - 4, lineHeight, Qt::AlignRight | Qt::AlignTop, QString::number(lineNumber));
		}

		block = block.next();
		top = bottom;
		lineHeight = m_editor->blockBoundingRect(block).height();
		bottom += lineHeight;
		++lineNumber;
	}
	painter.end();
}

void LineNumberWidget::mousePressEvent(QMouseEvent *event)
{
	event->accept();
	const QPoint p = m_editor->viewport()->mapFromGlobal(event->globalPos());
	const int lineNumber = m_editor->cursorForPosition(p).blockNumber() + 1;
	if (lineNumber <= 0)
		return;

	m_editor->toggleUserBookmark(lineNumber);
	update();
}

void LineNumberWidget::mouseReleaseEvent(QMouseEvent*)
{
	m_editor->setFocus();
}
