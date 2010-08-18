/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007 by Glad Deschrijver                                *
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

#include "loghighlighter.h"

LogHighlighter::LogHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(Qt::red);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "\\S*:\\d+:.*$" << "Undefined control sequence"
	    << "LaTeX Warning:" << "LaTeX Error:" << "Runaway argument?"
	    << "Missing character: .*!" << "Error:"
	    << "^\\[.*\\] Line \\d+: .*"; // error msg created by TikzPngPreviewer::getParsedLogText()
	foreach (const QString &pattern, keywordPatterns)
	{
		rule.pattern = QRegExp(pattern);
		rule.format = keywordFormat;
		m_highlightingRules.append(rule);
	}

	QTextCharFormat commandFormat;
	commandFormat.setForeground(Qt::darkBlue);
	commandFormat.setFontWeight(QFont::Bold);
	rule.pattern = QRegExp("^\\[[^\\]\\d][^\\]]*\\]");
	rule.format = commandFormat;
	m_highlightingRules.append(rule);

	m_statisticsFormat.setForeground(Qt::darkGray);
//	m_statisticsFormat.setFontPointSize(5.0);
	m_statisticsStartExpression = "Here is how much of TeX's memory you used:";
}

LogHighlighter::~LogHighlighter()
{
}

void LogHighlighter::highlightBlock(const QString &text)
{
	// Try each highlighting pattern and apply formatting if it matches
	foreach (const HighlightingRule &rule, m_highlightingRules)
	{
//		const QRegExp expression(rule.pattern);
//		int index = text.indexOf(expression);
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			const int length = expression.matchedLength();
			setFormat(index, length, rule.format);
//			index = text.indexOf(expression, index + length);
			index = expression.indexIn(text, index + length);
		}
	}
	// The current block state tracks multiline formatting
	setCurrentBlockState(0);
	int startIndex = text.indexOf(m_statisticsStartExpression); // Index to start highlighting statistics (if any)
	if (previousBlockState() == 1) // The previous block ended within statistics
		startIndex = 0;

	if (startIndex >= 0)
	{
		const int endIndex = text.size() - 1;
		const int statisticsLength = endIndex - startIndex + 1;
		setFormat(startIndex, statisticsLength, m_statisticsFormat);
		// Make the next invocation of this routine aware that it should continue highlighting in statistics format
		setCurrentBlockState(1);
	}
}
