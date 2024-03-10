/***************************************************************************
 *   Copyright (C) 2018-2020                                               *
 *     by Joao Carreira <jfmcarreira@gmail.com>                            *
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

#ifndef TIKZKTEXTEDITORVIEW_H
#define TIKZKTEXTEDITORVIEW_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <KTextEditor/Document>
#include <KTextEditor/View>

class TikzKTextEditorCompletion;

#include "tikzeditorviewabstract.h"

class TikzKTextEditorView : public TikzEditorViewAbstract
{
    Q_OBJECT

public:
    explicit TikzKTextEditorView(QWidget *parent = 0);
    virtual ~TikzKTextEditorView();

    KTextEditor::Document *editor();
    KTextEditor::Document *document();
    QString text() override;
    KTextEditor::View *view();

    void updateCompleter(bool useCompletion, const QStringList &words) override;
    void clear() override;
    bool isEmpty() override;
    bool isModified() override;
    void setModified(bool value) override;

Q_SIGNALS:
    void documentUrlChanged(const QUrl &url);
    void modificationChanged(bool changed);
    void contentsChanged();
    void cursorPositionChanged(int row, int col);
    void showStatusMessage(const QString &message, int timeout = 3000);
    void setSearchFromBegin(bool searchFromBegin);
    void focusIn();
    void focusOut();

private Q_SLOTS:
    void setDocumentModified(KTextEditor::Document *);

private:
    KTextEditor::Document *m_currentDoc;
    KTextEditor::View *m_documentView;
    TikzKTextEditorCompletion *m_completion;
};

#endif
