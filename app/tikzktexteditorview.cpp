/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013 by Glad Deschrijver  *
 *     <glad.deschrijver@gmail.com>                                        *
 *   Copyright (C) 2013 by João Carreira <jfmcarreira@gmail.com>           *
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

#include "tikzktexteditorview.h"
#include "tikzktexteditorcompletion.h"

#include <KTextEditor/Editor>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/CodeCompletionInterface>

#include <QWidget>
#include <QVBoxLayout>

TikzKTextEditorView::TikzKTextEditorView(QWidget *parent) : TikzEditorViewAbstract(parent)
{

    m_currentDoc = KTextEditor::Editor::instance()->createDocument(this);
    // create a widget to display the document
    m_documentView = m_currentDoc->createView(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_documentView);

    m_currentDoc->setMode(QStringLiteral("LaTeX"));

    connect(m_currentDoc, &KTextEditor::Document::modifiedChanged, this,
            &TikzKTextEditorView::setDocumentModified);

    connect(m_documentView, &KTextEditor::View::focusIn, this, &TikzKTextEditorView::focusIn);
    connect(m_documentView, &KTextEditor::View::focusOut, this, &TikzKTextEditorView::focusOut);

    connect(m_currentDoc, &KTextEditor::Document::textChanged, this,
            &TikzKTextEditorView::contentsChanged);

    connect(m_currentDoc, &KTextEditor::Document::urlChanged, this,
            &TikzKTextEditorView::documentUrlChanged);

    KTextEditor::CodeCompletionInterface *cci =
            qobject_cast<KTextEditor::CodeCompletionInterface *>(m_documentView);
    if (cci) {
        m_completion = new TikzKTextEditorCompletion(this);
        cci->registerCompletionModel(m_completion);
    }
}

TikzKTextEditorView::~TikzKTextEditorView() { }

KTextEditor::Document *TikzKTextEditorView::editor()
{
    return document();
}

KTextEditor::Document *TikzKTextEditorView::document()
{
    return m_currentDoc;
}

QString TikzKTextEditorView::text()
{
    return m_currentDoc->text();
}

KTextEditor::View *TikzKTextEditorView::view()
{
    return m_documentView;
}

void TikzKTextEditorView::setDocumentModified(KTextEditor::Document *doc)
{
    Q_EMIT modificationChanged(doc->isModified());
}

void TikzKTextEditorView::updateCompleter(bool useCompletion, const QStringList &words)
{
    m_completion->updateCompleter(useCompletion, words);
}

void TikzKTextEditorView::clear()
{
    editor()->clear();
}

bool TikzKTextEditorView::isEmpty()
{
    return document()->isEmpty();
}

bool TikzKTextEditorView::isModified()
{
    return document()->isModified();
}

void TikzKTextEditorView::setModified(bool value)
{
    return document()->setModified(value);
}
