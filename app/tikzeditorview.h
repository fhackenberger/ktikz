/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Glad Deschrijver                    *
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

#include <QWidget>
//#include "tikzcommandinserter.h"

class QAction;
class QCompleter;
class QMenu;
//class QSyntaxHighlighter;
class QPlainTextEdit;
class QToolBar;
class GoToLineWidget;
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
	QMenu *menu();
	QToolBar *toolBar();
	void applySettings();
	void setLine(const QString &line);
	void setCompleter (QCompleter *completer);

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
	void editComment();
	void editUncomment();
	void editFind();
	void editFindNext();
	void editFindPrevious();
	void editReplace();
	bool search(const QString &text, bool isCaseSensitive = false, bool findWholeWords = false, bool forward = true, bool startAtCursor = true);
	void replace(const QString &replacement);
	void replace(const QString &text, const QString &replacement, bool isCaseSensitive = false, bool findWholeWords = false, bool forward = true, bool startAtCursor = true);
	void replaceAll(const QString &text, const QString &replacement, bool isCaseSensitive = false, bool findWholeWords = false, bool forward = true, bool startAtCursor = true);

private:
	void setFont(const QFont &editorFont);
	void createActions();
	void setLine(int lineNumber);

	QWidget *m_parentWidget;
	TikzEditor *m_tikzEditor;
	ReplaceWidget *m_replaceWidget;
	ReplaceCurrentWidget *m_replaceCurrentWidget;
	GoToLineWidget *m_goToLineWidget;

//	TikzHighlighter *tikzHighlighter;

//	TikzCommandInserter *commandInserter;

	QAction *m_undoAction;
	QAction *m_redoAction;
	QAction *m_cutAction;
	QAction *m_copyAction;
	QAction *m_pasteAction;
	QAction *m_selectAllAction;
	QList<QAction*> m_editActions;

//	int customHighlighting;
};

#endif
