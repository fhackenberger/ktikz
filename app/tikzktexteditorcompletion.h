/***************************************************************************
 *   Copyright (C) 2015 by Eike Hein <hein@kde.org>                        *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef TIKZKTEXTEDITORCOMPLETION_H
#define TIKZKTEXTEDITORCOMPLETION_H

#include <KTextEditor/CodeCompletionModel>
#include <KTextEditor/CodeCompletionModelControllerInterface>

#include <QIcon>
#include <QModelIndexList>
#include <QStandardItemModel>
#include <QStringList>
#include <QUrl>

namespace KTextEditor {
    class Document;
    class View;
}


class TikzKTextEditorCompletion
	: public KTextEditor::CodeCompletionModel
	, public KTextEditor::CodeCompletionModelControllerInterface
{
	Q_OBJECT

	Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)

public:
	TikzKTextEditorCompletion(QObject* parent);
	~TikzKTextEditorCompletion();

	void updateCompleter(bool useCompletion, const QStringList &words);

	//bool shouldStartCompletion(KTextEditor::View *view, const QString &insertedText, bool userInsertion, const KTextEditor::Cursor &position) Q_DECL_OVERRIDE;
	KTextEditor::Range completionRange(KTextEditor::View *view, const KTextEditor::Cursor &position) Q_DECL_OVERRIDE;
	//QString filterString(KTextEditor::View *view, const KTextEditor::Range &range, const KTextEditor::Cursor &position) Q_DECL_OVERRIDE;

	void completionInvoked(KTextEditor::View *view, const KTextEditor::Range &range, KTextEditor::CodeCompletionModel::InvocationType it) Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

private:

	void findMatches();

	bool m_useCompletion;
	QStringList m_wordsList;
	/**
	 * model with matching data
	 */
	QStandardItemModel m_matches;

	/**
	 * automatic invocation?
	 */
	bool m_automatic;
};

#endif

