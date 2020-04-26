/***************************************************************************
 *   Copyright (C) 2012 Christoph Cullmann <cullmann@kde.org>              *
 *   Copyright (C) 2003 Anders Lund <anders.lund@lund.tdcadsl.dk>          *
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

#include "tikzktexteditorcompletion.h"

#include <QProcessEnvironment>
#include <QTemporaryFile>
#include <QTextStream>

#include <KTextEditor/Cursor>
#include <KTextEditor/Document>
#include <KTextEditor/View>


TikzKTextEditorCompletion::TikzKTextEditorCompletion(QObject* parent)
		: KTextEditor::CodeCompletionModel(parent)
{
	m_wordsList.clear();
}

TikzKTextEditorCompletion::~TikzKTextEditorCompletion()
{


}

void TikzKTextEditorCompletion::updateCompleter(bool useCompletion, const QStringList &words)
{
	m_useCompletion = useCompletion;
	m_wordsList = words;
	findMatches();
}

void TikzKTextEditorCompletion::findMatches()
{
	m_matches.clear();
	for( int i = 0; i < m_wordsList.size(); i++ )
	{
		QStandardItem* item = new QStandardItem();
		item->setData( m_wordsList.at(i) , Qt::DisplayRole);
		if( m_wordsList.at(i).startsWith(QStringLiteral("\\")) )
		{
			item->setData(QIcon::fromTheme(QStringLiteral("code-function")), Qt::DecorationRole);
		}
		m_matches.appendRow( item );
	}
}


//bool TikzKTextEditorCompletion::shouldStartCompletion(KTextEditor::View *view, const QString &insertedText, bool userInsertion, const KTextEditor::Cursor &position)
//{
//	if( !m_useCompletion )
//		return false;
//	if( insertedText.size() >= 3 )
//	{
//		return true;
//	}
//	if( insertedText.size() == 1 )
//	{
//		if( insertedText.at(0) == '{'
//				|| insertedText.at(0) == '['
//				|| insertedText.at(0) == '\\' )
//		return true;
//	}
//	return false;
//}


KTextEditor::Range TikzKTextEditorCompletion::completionRange(KTextEditor::View *view, const KTextEditor::Cursor &position)
{
	KTextEditor::Range range = view->document()->wordRangeAt( position );
	return range;
}

void TikzKTextEditorCompletion::completionInvoked(KTextEditor::View *view, const KTextEditor::Range &range, KTextEditor::CodeCompletionModel::InvocationType it)
{
	setRowCount(m_matches.rowCount());
}

QVariant TikzKTextEditorCompletion::data(const QModelIndex &index, int role) const
{

	if (!index.isValid() || index.row() >= m_matches.rowCount()) {
		return QVariant();
	}

	const QStandardItem *match = m_matches.item(index.row());

	if(index.column() == KTextEditor::CodeCompletionModel::Name && role == Qt::DisplayRole)
	{
		QString name = match->data( Qt::DisplayRole ).toString();
		return name;
	}
	if(index.column() == KTextEditor::CodeCompletionModel::Icon && role == Qt::DecorationRole)
	{
		return match->data( Qt::DecorationRole );
	}

	switch (role)
	{
	case KTextEditor::CodeCompletionModel::CompletionRole:
	{
		int p = KTextEditor::CodeCompletionModel::NoProperty;
		return p;
	}
	case KTextEditor::CodeCompletionModel::BestMatchesCount:
	{
		return 1;
	}
	case KTextEditor::CodeCompletionModel::ArgumentHintDepth:
	{
		return 0;
	}
	case KTextEditor::CodeCompletionModel::GroupRole:
	{
		break;
	}
	case KTextEditor::CodeCompletionModel::IsExpandable:
	{
		// I like the green arrow
		return false;
	}
	case KTextEditor::CodeCompletionModel::ExpandingWidget:
	{
		// TODO well implementation in DCD is missing
		break;
	}
	}

	return QVariant();
}
