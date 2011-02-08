/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2011 by Glad Deschrijver                          *
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

#include "tikzeditorhighlighter.h"

#include <QApplication>
#include "tikzcommandinserter.h"

TikzHighlighter::TikzHighlighter(TikzCommandInserter *commandInserter, QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	m_commandInserter = commandInserter;

	if (commandInserter)
	{
		m_highlightingRules << commandInserter->getHighlightingRules();
		m_highlightTypeNames << commandInserter->getHighlightTypeNames();
	}

	// add highlighting for environments and comments
	HighlightingRule rule;
	const int currentIndex = m_highlightTypeNames.size();
	m_highlightTypeNames << "Environments" << "Comments";
	// environments
	QStringList keywordPatterns;
	keywordPatterns << "\\\\begin\\{[^\\}]*\\}" << "\\\\end\\{[^\\}]*\\}";
	foreach (const QString &pattern, keywordPatterns)
	{
		rule.type = m_highlightTypeNames.at(currentIndex);
		rule.pattern = QRegExp(pattern);
		m_highlightingRules.append(rule);
	}
	// comments
	rule.type = m_highlightTypeNames.at(currentIndex + 1);
	rule.pattern = QRegExp("%[^\n]*");
	m_highlightingRules.append(rule);

	m_formatList = getDefaultHighlightFormats();
}

TikzHighlighter::~TikzHighlighter()
{
}

QMap<QString, QTextCharFormat> TikzHighlighter::getDefaultHighlightFormats()
{
	QMap<QString, QTextCharFormat> formatList;
	if (m_commandInserter)
		formatList = m_commandInserter->getDefaultHighlightFormats();
	const int currentIndex = m_highlightTypeNames.size() - 2;

	// format for environments
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFont(qApp->font());
	keywordFormat.setFontWeight(QFont::Bold);
	formatList[m_highlightTypeNames.at(currentIndex)] = keywordFormat;

	// format for comments
	QTextCharFormat commentFormat;
	commentFormat.setForeground(Qt::gray);
	commentFormat.setFont(qApp->font());
	commentFormat.setFontWeight(QFont::Normal);
	formatList[m_highlightTypeNames.at(currentIndex + 1)] = commentFormat;

	return formatList;
}

QStringList TikzHighlighter::getTranslatedHighlightTypeNames()
{
	QStringList translatedHighlightTypeNames;
	if (m_commandInserter)
		translatedHighlightTypeNames = m_commandInserter->getTranslatedHighlightTypeNames();
	translatedHighlightTypeNames << tr("Environments") << tr("Comments");
	return translatedHighlightTypeNames;
}

QStringList TikzHighlighter::getHighlightTypeNames()
{
	return m_highlightTypeNames;
}

QMap<QString, QTextCharFormat> TikzHighlighter::getTextCharFormats()
{
	return m_formatList;
}

void TikzHighlighter::setTextCharFormats(const QMap<QString, QTextCharFormat> &formatList)
{
	m_formatList = formatList;
}

void TikzHighlighter::highlightBlock(const QString &text)
{
	// Try each highlighting pattern and apply formatting if it matches
	foreach (const HighlightingRule &rule, m_highlightingRules)
	{
		if (rule.pattern.isEmpty()) // match the insertion string
		{
			int index = text.indexOf(rule.matchString);
			while (index >= 0)
			{
				const int length = rule.matchString.length();
				if (index == 0 || text.at(index - 1) != '\\')
					setFormat(index, length, m_formatList[rule.type]);
				index = text.indexOf(rule.matchString, index + length);
			}
		}
		else // match the pattern
		{
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while (index >= 0)
			{
				const int length = expression.matchedLength();
				if (index == 0 || text.at(index-1) != '\\')
					setFormat(index, length, m_formatList[rule.type]);
				index = expression.indexIn(text, index + length);
			}
		}
	}
	setCurrentBlockState(0);
}
