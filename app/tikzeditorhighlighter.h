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

#ifndef TIKZEDITORHIGHLIGHTER_H
#define TIKZEDITORHIGHLIGHTER_H

#include <QtGui/QSyntaxHighlighter>

struct HighlightingRule
{
    QString type; /// The name of the formatting type
    QString matchString; /// The string to match for formatting; only used when pattern is empty
    QRegExp pattern; /// The pattern to match for formatting; overrides usage of matchString
    bool isRegExp;
};

/** A simple highlighter for the TikZ graphics programming language
 * @author Florian Hackenberger <florian@hackenberger.at>
 */
class TikzHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit TikzHighlighter(QTextDocument *parent = 0);
    virtual ~TikzHighlighter();

    void setHighlightingRules(const QVector<HighlightingRule> &highlightingRules);
    static QMap<QString, QTextCharFormat> getDefaultHighlightFormats();
    static QStringList getTranslatedHighlightTypeNames();
    static QStringList getHighlightTypeNames();
    void applySettings();

protected:
    /** Implements QSyntaxHighlighter::highlightBlock()
     * @see QSyntaxHighlighter::highlightBlock()
     */
    void highlightBlock(const QString &text) override;

private:
    /// All highlighting rules with their formatting for easy iteration
    QVector<HighlightingRule> m_highlightingRules;

    QMap<QString, QTextCharFormat> m_formatList;
    QStringList m_highlightTypeNames;
};

#endif
