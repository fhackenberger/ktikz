/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2011, 2012 by Glad Deschrijver                    *
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

#include <QtCore/QSettings>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "tikzcommandinserter.h"

TikzHighlighter::TikzHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
}

TikzHighlighter::~TikzHighlighter()
{
}

void TikzHighlighter::setHighlightingRules(const QVector<HighlightingRule> &highlightingRules)
{
	m_highlightingRules << highlightingRules;

	// add highlighting for environments and comments
	HighlightingRule rule;
	QStringList highlightTypeNames = getHighlightTypeNames();
	const int currentIndex = highlightTypeNames.size() - 2;
	// environments
	QStringList keywordPatterns;
	keywordPatterns << "\\\\begin\\{[^\\}]*\\}" << "\\\\end\\{[^\\}]*\\}";
	foreach (const QString &pattern, keywordPatterns)
	{
		rule.type = highlightTypeNames.at(currentIndex);
		rule.pattern = QRegExp(pattern);
		m_highlightingRules.append(rule);
	}
	// comments
	rule.type = highlightTypeNames.at(currentIndex + 1);
	rule.pattern = QRegExp("%[^\n]*");
	m_highlightingRules.append(rule);

//	m_formatList = getDefaultHighlightFormats();
}

/***************************************************************************/

QMap<QString, QTextCharFormat> TikzHighlighter::getDefaultHighlightFormats()
{
	QMap<QString, QTextCharFormat> formatList = TikzCommandInserter::getDefaultHighlightFormats();
	QStringList highlightTypeNames = getHighlightTypeNames();
	const int currentIndex = highlightTypeNames.size() - 2;

	// format for environments
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFont(qApp->font());
	keywordFormat.setFontWeight(QFont::Bold);
	formatList[highlightTypeNames.at(currentIndex)] = keywordFormat;

	// format for comments
	QTextCharFormat commentFormat;
	commentFormat.setForeground(Qt::gray);
	commentFormat.setFont(qApp->font());
	commentFormat.setFontWeight(QFont::Normal);
	formatList[highlightTypeNames.at(currentIndex + 1)] = commentFormat;

	return formatList;
}

QStringList TikzHighlighter::getTranslatedHighlightTypeNames()
{
	QStringList translatedHighlightTypeNames = TikzCommandInserter::getTranslatedHighlightTypeNames();
	translatedHighlightTypeNames << tr("Environments") << tr("Comments");
	return translatedHighlightTypeNames;
}

QStringList TikzHighlighter::getHighlightTypeNames()
{
	QStringList highlightTypeNames = TikzCommandInserter::getHighlightTypeNames();
	highlightTypeNames << "Environments" << "Comments";
	return highlightTypeNames;
}

/***************************************************************************/

void TikzHighlighter::applySettings()
{
	QSettings settings;
	settings.beginGroup("Highlighting");
	const bool customHighlighting = settings.value("Customize", true).toBool();

	m_formatList.clear();
	m_formatList = getDefaultHighlightFormats();

	if (customHighlighting)
	{
		const int numOfRules = settings.value("Number", 0).toInt();
		for (int i = 0; i < numOfRules; ++i)
		{
			const QString name = settings.value("Item" + QString::number(i) + "/Name").toString();
			const QString colorName = settings.value("Item" + QString::number(i) + "/Color").toString();
			const QString fontName = settings.value("Item" + QString::number(i) + "/Font").toString();
			QFont font;
			font.fromString(fontName);
			QTextCharFormat format;
			format.setForeground(QBrush(QColor(colorName)));
			format.setFont(font);
			m_formatList[name] = format;
		}
	}
	settings.endGroup();
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
				if (index == 0 || text.at(index - 1) != '\\') // avoid matching e.g. "node" as an option if in reality "\node" as a command is written
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
