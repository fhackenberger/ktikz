/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011 by Glad Deschrijver              *
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

#ifndef TIKZEDITORVIEW_H
#define TIKZEDITORVIEW_H

#include <QtGui/QWidget>
#include <QtGui/QTextDocument>
//#include "tikzcommandinserter.h"

class QAction;
class QMenu;
//class QSyntaxHighlighter;
class QPlainTextEdit;
class QToolBar;
class Action;
class GoToLineWidget;
class IndentWidget;
class ReplaceWidget;
class ReplaceCurrentWidget;
class TikzEditor;
//class TikzHighlighter;

class TikzEditorView : public QWidget
{
	Q_OBJECT

public:
	TikzEditorView(QWidget *parent = 0);
	virtual ~TikzEditorView();

	QPlainTextEdit *editor();
#ifndef KTIKZ_USE_KDE
	QMenu *editMenu();
	QMenu *bookmarksMenu();
	QToolBar *toolBar();
#endif
	void applySettings();
	void setLine(const QString &line);
	int lineNumber() const;
	void updateCompleter(bool useCompletion, const QStringList &words);

public slots:
	void goToLine(int lineNumber);

signals:
	void modificationChanged(bool changed);
	void contentsChanged();
	void cursorPositionChanged(int row, int col);
	void showStatusMessage(const QString &message, int timeout = 3000);
	void setSearchFromBegin(bool searchFromBegin);
	void focusIn();
	void focusOut();

private slots:
	void setPasteEnabled();
//	void showCursorPosition();
	void editGoToLine();
	void editIndent();
	void editUnindent();
	void editComment();
	void editUncomment();
	void editFind();
	void editFindNext();
	void editFindPrevious();
	void editReplace();
	void tabIndent(bool isUnindenting = false);
	void indent(QChar insertChar, int numOfInserts, bool isUnindenting = false);
	bool search(const QString &text, QTextDocument::FindFlags flags = 0, bool startAtCursor = true, bool continueFromBeginning = false);
	void search();
	void replace(const QString &replacement);
	void replace(const QString &text, const QString &replacement, QTextDocument::FindFlags flags = 0, bool startAtCursor = true);
	void replace();
	void replaceAll();

private:
	void setFont(const QFont &editorFont);
	void createActions();
	void initGoToLineWidget();
	void setLine(int lineNumber);
	void initIndentWidget();
	void openIndentWidget();
	void initReplaceWidgets();
	void openReplaceWidget();

	TikzEditor *m_tikzEditor;
	GoToLineWidget *m_goToLineWidget;
	IndentWidget *m_indentWidget;
	ReplaceWidget *m_replaceWidget;
	ReplaceCurrentWidget *m_replaceCurrentWidget;

	QString m_searchText;
	QString m_replaceText;
	QTextDocument::FindFlags m_flags;
	bool m_startAtCursor;

//	TikzHighlighter *tikzHighlighter;

//	TikzCommandInserter *commandInserter;

	QAction *m_undoAction;
	QAction *m_redoAction;
	QAction *m_cutAction;
	QAction *m_copyAction;
	QAction *m_pasteAction;
	QAction *m_selectAllAction;
	QList<QAction*> m_editActions;
	Action *m_setBookmarkAction;
	Action *m_previousBookmarkAction;
	Action *m_nextBookmarkAction;

//	int customHighlighting;
};

#endif
