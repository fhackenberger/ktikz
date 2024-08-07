/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2011, 2012, 2014 by Glad Deschrijver              *
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

#include <QDebug>
#include <QtCore/QSettings>
#include <QtCore/QRegularExpression>
#include <QtWidgets/QApplication>

#include "tikzeditorhighlighter.h"
#include "tikzcommandinserter.h"

TikzHighlighter::TikzHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) { }

TikzHighlighter::~TikzHighlighter() { }

void TikzHighlighter::setHighlightingRules(const QVector<HighlightingRule> &highlightingRules)
{
    m_highlightingRules << highlightingRules;

    // add highlighting for environments and comments
    HighlightingRule rule;
    QStringList highlightTypeNames = getHighlightTypeNames();
    const int currentIndex = highlightTypeNames.size() - 2;
    // environments
    QStringList keywordPatterns;
    keywordPatterns << QLatin1String("\\\\begin\\{[^\\}]*\\}")
                    << QLatin1String("\\\\end\\{[^\\}]*\\}");
    for (const auto &pattern : keywordPatterns) {
        rule.type = highlightTypeNames.at(currentIndex);
        rule.pattern = QRegularExpression(pattern);
        rule.isRegExp = true;
        m_highlightingRules.append(rule);
    }
    // comments
    rule.type = highlightTypeNames.at(currentIndex + 1);
    rule.pattern = QRegularExpression(QLatin1String("%[^\n]*"));
    rule.isRegExp = true;
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
    QStringList translatedHighlightTypeNames =
            TikzCommandInserter::getTranslatedHighlightTypeNames();
    translatedHighlightTypeNames << tr("Environments") << tr("Comments");
    return translatedHighlightTypeNames;
}

QStringList TikzHighlighter::getHighlightTypeNames()
{
    QStringList highlightTypeNames = TikzCommandInserter::getHighlightTypeNames();
    highlightTypeNames << QLatin1String("Environments") << QLatin1String("Comments");
    return highlightTypeNames;
}

/***************************************************************************/

void TikzHighlighter::applySettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("Highlighting"));
    const bool customHighlighting = settings.value(QLatin1String("Customize"), true).toBool();

    m_formatList.clear();
    m_formatList = getDefaultHighlightFormats();

    if (customHighlighting) {
        const int numOfRules = settings.value(QLatin1String("Number"), 0).toInt();
        for (int i = 0; i < numOfRules; ++i) {
            const QString name = settings.value(QLatin1String("Item") + QString::number(i)
                                                + QLatin1String("/Name"))
                                         .toString();
            const QString colorName = settings.value(QLatin1String("Item") + QString::number(i)
                                                     + QLatin1String("/Color"))
                                              .toString();
            const QString fontName = settings.value(QLatin1String("Item") + QString::number(i)
                                                    + QLatin1String("/Font"))
                                             .toString();
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

namespace {

int indexOf(const QString &text, const QString &tag, int startPos = 0)
{
    // valgrind --tool=callgrind --zero-before="MainWindow::loadUrl(Url const&)" gives a small win
    // for this function over "text.indexOf(tag, startPos);"
    const int textLength = text.length();
    const int tagLength = tag.length();
    for (int i = startPos, j = 0; i < textLength && j < tagLength; ++i, ++j) {
        if (text.at(i) != tag.at(j)) {
            j = -1;
            continue;
        }
        if (j == tagLength - 1)
            return i - tagLength + 1;
    }
    return -1;
}

} // anonymous namespace

void TikzHighlighter::highlightBlock(const QString &text)
{
    // Try each highlighting pattern and apply formatting if it matches
    // Having the outer loop loop over the highlighting rules and the inner loop over the text is
    // much faster than conversely
    for (const auto &rule : m_highlightingRules) {
        if (!rule.isRegExp) // match the insertion string
        {
            const int length = rule.matchString.length();
            //			int index = text.indexOf(rule.matchString);
            int index = indexOf(text, rule.matchString);
            while (index >= 0) {
                if (index == 0
                    || text.at(index - 1)
                            != QLatin1Char('\\')) // avoid matching e.g. "node" as an option if in
                                                  // reality "\node" as a command is written
                    setFormat(index, length, m_formatList[rule.type]);
                //				index = text.indexOf(rule.matchString, index +
                // length);
                index = indexOf(text, rule.matchString, index + length);
            }
        } else // match the pattern
        {
            auto m = rule.pattern.match(text);
            while (m.hasMatch()) {
                const int length = m.capturedLength();
                const int index = m.capturedStart();
                if (index == 0 || text.at(index - 1) != QLatin1Char('\\'))
                    setFormat(index, length, m_formatList[rule.type]);
                m = rule.pattern.match(text, m.capturedEnd());
            }
        }
    }
    setCurrentBlockState(0);
}
