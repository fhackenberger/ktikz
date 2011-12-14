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

#include "tikzeditorview.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>
#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtGui/QTextCursor>
#include <QtGui/QToolBar>

#include "editgotolinewidget.h"
#include "editindentwidget.h"
#include "editreplacewidget.h"
#include "editreplacecurrentwidget.h"
#include "tikzeditor.h"
//#include "tikzeditorhighlighter.h"
#include "../common/utils/action.h"
#include "../common/utils/icon.h"
#include "../common/utils/messagebox.h"
#include "../common/utils/standardaction.h"

TikzEditorView::TikzEditorView(QWidget *parent) : QWidget(parent)
{
	m_tikzEditor = new TikzEditor;
	m_tikzEditor->setWhatsThis(tr("<p>Enter your TikZ code here.  "
	                              "The code should begin with \\begin{tikzpicture} and end with "
	                              "\\end{tikzpicture}.</p>"));
/*
	commandInserter = new TikzCommandInserter(tikzEditor, this);
	tikzHighlighter = new TikzHighlighter(commandInserter, tikzEditor->document());
	tikzHighlighter->rehighlight(); // avoid that textEdit emits the signal contentsChanged() when it is still empty
	tikzController = new TikzPngPreviewer(textEdit);
*/

	// delay creating the following until they are actually needed
	m_replaceWidget = 0;
	m_replaceCurrentWidget = 0;
	m_goToLineWidget = 0;
	m_indentWidget = 0;

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	mainLayout->addWidget(m_tikzEditor);

	createActions();

	connect(m_tikzEditor->document(), SIGNAL(modificationChanged(bool)),
	        this, SIGNAL(modificationChanged(bool)));
	connect(m_tikzEditor->document(), SIGNAL(contentsChanged()),
	        this, SIGNAL(contentsChanged()));
	connect(m_tikzEditor, SIGNAL(cursorPositionChanged(int,int)),
	        this, SIGNAL(cursorPositionChanged(int,int)));
	connect(m_tikzEditor, SIGNAL(showStatusMessage(QString,int)),
	        this, SIGNAL(showStatusMessage(QString,int)));
	connect(m_tikzEditor, SIGNAL(tabIndent(bool)),
	        this, SLOT(tabIndent(bool)));

	connect(m_tikzEditor, SIGNAL(focusIn()),
	        this, SIGNAL(focusIn()));
	connect(m_tikzEditor, SIGNAL(focusOut()),
	        this, SIGNAL(focusOut()));
}

TikzEditorView::~TikzEditorView()
{
//	tikzHighlighter->deleteLater();
}

QPlainTextEdit *TikzEditorView::editor()
{
	return m_tikzEditor;
}

void TikzEditorView::setFont(const QFont &editorFont)
{
	m_tikzEditor->setFont(editorFont);
	m_tikzEditor->setTabStopWidth(m_tikzEditor->fontMetrics().width("    "));
}

void TikzEditorView::createActions()
{
	m_undoAction = StandardAction::undo(m_tikzEditor, SLOT(undo()), this);
	m_redoAction = StandardAction::redo(m_tikzEditor, SLOT(redo()), this);
	m_cutAction = StandardAction::cut(m_tikzEditor, SLOT(cut()), this);
	m_copyAction = StandardAction::copy(m_tikzEditor, SLOT(copy()), this);
	m_pasteAction = StandardAction::paste(m_tikzEditor, SLOT(paste()), this);
	m_selectAllAction = StandardAction::selectAll(m_tikzEditor, SLOT(selectAll()), this);
	m_undoAction->setStatusTip(tr("Undo the previous action"));
	m_redoAction->setStatusTip(tr("Redo the previous undone action"));
	m_cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
	m_copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
	m_pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
	m_selectAllAction->setStatusTip(tr("Select all the content"));
	m_undoAction->setWhatsThis(tr("<p>Undo the previous action.</p>"));
	m_redoAction->setWhatsThis(tr("<p>Redo the previous undone action.</p>"));
	m_cutAction->setWhatsThis(tr("<p>Cut the current selection's contents to the clipboard.</p>"));
	m_copyAction->setWhatsThis(tr("<p>Copy the current selection's contents to the clipboard.</p>"));
	m_pasteAction->setWhatsThis(tr("<p>Paste the clipboard's contents into the current selection.</p>"));
	m_selectAllAction->setWhatsThis(tr("<p>Select all the content.</p>"));

	Action *action;
	action = new Action(Icon("format-indent-more"), tr("&Indent..."), this, "edit_indent");
	action->setShortcut(tr("Ctrl+I", "Edit|Indent"));
	action->setStatusTip(tr("Indent the current line or selection"));
	action->setWhatsThis(tr("<p>Indent the current line or selection.</p>"));
	connect(action, SIGNAL(triggered()), this, SLOT(editIndent()));
	m_editActions.append(action);

	action = new Action(Icon("format-indent-less"), tr("Unind&ent..."), this, "edit_unindent");
	action->setShortcut(tr("Ctrl+Shift+I", "Edit|Unindent"));
	action->setStatusTip(tr("Unindent the current line or selection"));
	action->setWhatsThis(tr("<p>Unindent the current line or selection.</p>"));
	connect(action, SIGNAL(triggered()), this, SLOT(editUnindent()));
	m_editActions.append(action);

	action = new Action(tr("C&omment"), this, "edit_comment");
	action->setShortcut(tr("Ctrl+D", "Edit|Comment"));
	action->setStatusTip(tr("Comment the current line or selection"));
	action->setWhatsThis(tr("<p>Comment the current line or selection.</p>"));
	connect(action, SIGNAL(triggered()), this, SLOT(editComment()));
	m_editActions.append(action);

	action = new Action(tr("Unco&mment"), this, "edit_uncomment");
	action->setShortcut(tr("Ctrl+Shift+D", "Edit|Uncomment"));
	action->setStatusTip(tr("Uncomment the current line or selection"));
	action->setWhatsThis(tr("<p>Uncomment the current line or selection.</p>"));
	connect(action, SIGNAL(triggered()), this, SLOT(editUncomment()));
	m_editActions.append(action);

	action = new Action(this);
	action->setSeparator(true);
	m_editActions.append(action);

	m_editActions.append(StandardAction::find(this, SLOT(editFind()), this));
	m_editActions.append(StandardAction::findNext(this, SLOT(editFindNext()), this));
	m_editActions.append(StandardAction::findPrev(this, SLOT(editFindPrevious()), this));
	m_editActions.append(StandardAction::replace(this, SLOT(editReplace()), this));
	m_editActions.append(StandardAction::gotoLine(this, SLOT(editGoToLine()), this));
	m_editActions.at(4)->setStatusTip(tr("Look up a piece of text in the document"));
	m_editActions.at(5)->setStatusTip(tr("Search the next occurrence of a text"));
	m_editActions.at(6)->setStatusTip(tr("Search the previous occurrence of a text"));
	m_editActions.at(7)->setStatusTip(tr("Search and replace a piece of text in the document"));
	m_editActions.at(8)->setStatusTip(tr("Go to a certain line in the document"));
	m_editActions.at(4)->setWhatsThis(tr("<p>Look up a piece of text in the document.</p>"));
	m_editActions.at(5)->setWhatsThis(tr("<p>Search the next occurrence of a text.</p>"));
	m_editActions.at(6)->setWhatsThis(tr("<p>Search the previous occurrence of a text.</p>"));
	m_editActions.at(7)->setWhatsThis(tr("<p>Search and replace a piece of text in the document.</p>"));
	m_editActions.at(8)->setWhatsThis(tr("<p>Go to a certain line in the document.</p>"));

	m_undoAction->setEnabled(false);
	m_redoAction->setEnabled(false);
	m_cutAction->setEnabled(false);
	m_copyAction->setEnabled(false);
	m_pasteAction->setEnabled(m_tikzEditor->canPaste());

	connect(m_tikzEditor, SIGNAL(undoAvailable(bool)),
	        m_undoAction, SLOT(setEnabled(bool)));
	connect(m_tikzEditor, SIGNAL(redoAvailable(bool)),
	        m_redoAction, SLOT(setEnabled(bool)));
	connect(m_tikzEditor, SIGNAL(copyAvailable(bool)),
	        m_cutAction, SLOT(setEnabled(bool)));
	connect(m_tikzEditor, SIGNAL(copyAvailable(bool)),
	        m_copyAction, SLOT(setEnabled(bool)));
	connect(QApplication::clipboard(), SIGNAL(dataChanged()),
	        this, SLOT(setPasteEnabled()));

	m_setBookmarkAction = new Action(Icon("bookmark-new"), tr("Set &Bookmark"), this, "bookmarks_set");
	m_setBookmarkAction->setShortcut(tr("Ctrl+B", "Bookmarks|Set"));
	m_setBookmarkAction->setStatusTip(tr("Set or unset a bookmark at the current line"));
	m_setBookmarkAction->setWhatsThis(tr("<p>Set or unset a bookmark at the current line.</p>"));
	connect(m_setBookmarkAction, SIGNAL(triggered()), m_tikzEditor, SLOT(toggleUserBookmark()));

	m_previousBookmarkAction = new Action(tr("&Previous Bookmark"), this, "bookmarks_prev");
	m_previousBookmarkAction->setShortcut(tr("Alt+Up", "Bookmarks|Previous"));
	m_previousBookmarkAction->setStatusTip(tr("Go to the previous bookmark"));
	m_previousBookmarkAction->setWhatsThis(tr("<p>Go to the previous bookmark.</p>"));
	connect(m_previousBookmarkAction, SIGNAL(triggered()), m_tikzEditor, SLOT(previousUserBookmark()));

	m_nextBookmarkAction = new Action(tr("&Next Bookmark"), this, "bookmarks_next");
	m_nextBookmarkAction->setShortcut(tr("Alt+Down", "Bookmarks|Next"));
	m_nextBookmarkAction->setStatusTip(tr("Go to the next bookmark"));
	m_nextBookmarkAction->setWhatsThis(tr("<p>Go to the next bookmark.</p>"));
	connect(m_nextBookmarkAction, SIGNAL(triggered()), m_tikzEditor, SLOT(nextUserBookmark()));
}

#ifndef KTIKZ_USE_KDE
QMenu *TikzEditorView::editMenu()
{
	QMenu *editMenu = new QMenu(tr("&Edit"), parentWidget());
	editMenu->addAction(m_undoAction);
	editMenu->addAction(m_redoAction);
	editMenu->addSeparator();
	editMenu->addAction(m_cutAction);
	editMenu->addAction(m_copyAction);
	editMenu->addAction(m_pasteAction);
	editMenu->addSeparator();
	editMenu->addAction(m_selectAllAction);
	editMenu->addSeparator();
	editMenu->addActions(m_editActions);
	return editMenu;
}

QMenu *TikzEditorView::bookmarksMenu()
{
	QMenu *bookmarksMenu = new QMenu(tr("&Bookmarks"), parentWidget());
	bookmarksMenu->addAction(m_setBookmarkAction);
	bookmarksMenu->addAction(m_previousBookmarkAction);
	bookmarksMenu->addAction(m_nextBookmarkAction);
	return bookmarksMenu;
}

QToolBar *TikzEditorView::toolBar()
{
	QToolBar *editToolBar = new QToolBar(tr("Edit"), parentWidget());
	editToolBar->setObjectName("EditToolBar");
	editToolBar->addAction(m_undoAction);
	editToolBar->addAction(m_redoAction);
//	editToolBar->addAction(m_cutAction);
//	editToolBar->addAction(m_copyAction);
//	editToolBar->addAction(m_pasteAction);
	return editToolBar;
}
#endif

void TikzEditorView::setPasteEnabled()
{
	m_pasteAction->setEnabled(m_tikzEditor->canPaste());
}

void TikzEditorView::applySettings()
{
	QSettings settings(ORGNAME, APPNAME);

	settings.beginGroup("Editor");
	// set editor font
	m_tikzEditor->setShowLineNumberArea(settings.value("ShowLineNumberArea", true).toBool());
	QFont editorFont;
	editorFont.fromString(settings.value("Font", qApp->font().toString()).toString());
	setFont(editorFont);

	// set colors
	m_tikzEditor->setShowWhiteSpaces(settings.value("ShowWhiteSpaces", false).toBool());
	m_tikzEditor->setShowTabulators(settings.value("ShowTabulators", false).toBool());
	m_tikzEditor->setShowMatchingBrackets(settings.value("ShowMatchingBrackets", true).toBool());
	m_tikzEditor->setWhiteSpacesColor(settings.value("ColorWhiteSpaces", Qt::gray).value<QColor>());
	m_tikzEditor->setTabulatorsColor(settings.value("ColorTabulators", Qt::gray).value<QColor>());
	m_tikzEditor->setMatchingColor(settings.value("ColorMatchingBrackets", Qt::yellow).value<QColor>());
	settings.endGroup();

/*
	tikzHighlighter->applySettings();
	tikzHighlighter->rehighlight();
*/
}

/***************************************************************************/

/*
void TikzEditorView::showCursorPosition()
{
	QTextCursor cursor = m_tikzEditor->textCursor();
	emit cursorPositionChanged(cursor.blockNumber() + 1, cursor.position() - cursor.block().position() + 1);
}
*/

/***************************************************************************/
/* Go to line */

void TikzEditorView::initGoToLineWidget()
{
	m_goToLineWidget = new GoToLineWidget(this);
	layout()->addWidget(m_goToLineWidget);

	connect(m_goToLineWidget, SIGNAL(goToLine(int)),
	        this, SLOT(goToLine(int)));
	connect(m_goToLineWidget, SIGNAL(focusEditor()),
	        m_tikzEditor, SLOT(setFocus()));
}

void TikzEditorView::setLine(const QString &line)
{
	bool ok;
	const int lineNumber = line.toInt(&ok, 10);
	if (ok)
		setLine(lineNumber);
}


void TikzEditorView::setLine(int lineNumber)
{
	if (m_indentWidget)
		m_indentWidget->setVisible(false);
	if (m_replaceWidget)
	{
		m_replaceWidget->setVisible(false);
		m_replaceCurrentWidget->setVisible(false);
	}
	if (!m_goToLineWidget)
		initGoToLineWidget();
	m_goToLineWidget->setMaximumValue(m_tikzEditor->numOfLines());
	m_goToLineWidget->setValue(lineNumber);
	m_goToLineWidget->setVisible(true);
	m_goToLineWidget->setFocus();
}

void TikzEditorView::goToLine(int lineNumber)
{
	m_goToLineWidget->setVisible(false);
	m_tikzEditor->goToLine(lineNumber);
	m_tikzEditor->setFocus();
}

void TikzEditorView::editGoToLine()
{
	int lineNumber = m_tikzEditor->textCursor().blockNumber() + 1;
	if (lineNumber < 1)
		lineNumber = 1;
	setLine(lineNumber);
}

int TikzEditorView::lineNumber() const
{
	int lineNumber = m_tikzEditor->textCursor().blockNumber() + 1;
	return lineNumber < 1 ? 1 : lineNumber;
}

/***************************************************************************/

void TikzEditorView::initIndentWidget()
{
	m_indentWidget = new IndentWidget(this);
	layout()->addWidget(m_indentWidget);

	connect(m_indentWidget, SIGNAL(hidden()),
	        m_tikzEditor, SLOT(setFocus()));
	connect(m_indentWidget, SIGNAL(indent(QChar,int,bool)),
	        this, SLOT(indent(QChar,int,bool)));
}

void TikzEditorView::openIndentWidget()
{
	if (m_replaceWidget)
	{
		m_replaceWidget->setVisible(false);
		m_replaceCurrentWidget->setVisible(false);
	}
	if (m_goToLineWidget)
		m_goToLineWidget->setVisible(false);
	if (!m_indentWidget)
		initIndentWidget();
	m_indentWidget->setVisible(true);
	m_indentWidget->setFocus();
}

void TikzEditorView::editIndent()
{
	openIndentWidget();
	m_indentWidget->setUnindenting(false);
}

void TikzEditorView::editUnindent()
{
	openIndentWidget();
	m_indentWidget->setUnindenting(true);
}

void TikzEditorView::tabIndent(bool isUnindenting)
{
	indent(m_indentWidget->insertChar(), m_indentWidget->numOfInserts(), isUnindenting);
}

void TikzEditorView::indent(QChar insertChar, int numOfInserts, bool isUnindenting)
{
	m_indentWidget->setVisible(false);

	bool go = true;
	QTextCursor textCursor = m_tikzEditor->textCursor();
	if (textCursor.hasSelection())
	{
		textCursor.beginEditBlock();
		const int start = textCursor.selectionStart();
		int end = textCursor.selectionEnd();
		textCursor.setPosition(start, QTextCursor::MoveAnchor);
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		if (!isUnindenting)
		{
			while (textCursor.position() < end && go)
			{
				for (int i = 0; i < numOfInserts; ++i)
				{
					textCursor.insertText(insertChar);
					++end; // when a character is inserted, textCursor.selectionEnd() is shifted by 1 character, in order to let the test in the while-loop behave correctly, we must increment end
				}
				go = textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
			}
		}
		else
		{
			while (textCursor.position() < end && go)
			{
				for (int i = 0; i < numOfInserts; ++i)
				{
					if (textCursor.atBlockEnd()) // when the line is empty go to the next line instead of selecting the newline character (possibly together with characters of the next line)
						break;
					textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
					if (textCursor.selectedText() == insertChar)
					{
						textCursor.removeSelectedText();
						--end; // when a character is removed, textCursor.selectionEnd() is shifted by 1 character, in order to let the test in the while-loop behave correctly, we must decrement end
					}
				}
				go = textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
			}
		}
		textCursor.endEditBlock();
	}
	else
	{
		textCursor.beginEditBlock();
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		if (!isUnindenting)
		{
			for (int i = 0; i < numOfInserts; ++i)
				textCursor.insertText(insertChar);
		}
		else
		{
			for (int i = 0; i < numOfInserts; ++i)
			{
				textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
				if (textCursor.selectedText() == insertChar)
					textCursor.removeSelectedText();
			}
		}
		textCursor.endEditBlock();
	}
	m_tikzEditor->setFocus();
}

/***************************************************************************/

void TikzEditorView::editComment()
{
	bool go = true;
	QTextCursor textCursor = m_tikzEditor->textCursor();
	if (textCursor.hasSelection())
	{
		textCursor.beginEditBlock();
		const int start = textCursor.selectionStart();
		int end = textCursor.selectionEnd();
		textCursor.setPosition(start, QTextCursor::MoveAnchor);
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		while (textCursor.position() < end && go)
		{
			textCursor.insertText("% ");
			++end;
			++end;
			go = textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
		}
		textCursor.endEditBlock();
	}
	else
	{
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		textCursor.insertText("% ");
	}
}

void TikzEditorView::editUncomment()
{
	bool go = true;
	QTextCursor textCursor = m_tikzEditor->textCursor();
	if (textCursor.hasSelection())
	{
		textCursor.beginEditBlock();
		const int start = textCursor.selectionStart();
		int end = textCursor.selectionEnd() - 2;
		textCursor.setPosition(start, QTextCursor::MoveAnchor);
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		while (textCursor.position() < end && go)
		{
			textCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
			if (textCursor.selectedText() == QLatin1String("% "))
			{
				textCursor.removeSelectedText();
				--end;
			}
			go = textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
		}
		textCursor.endEditBlock();
	}
	else
	{
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		textCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
		if (textCursor.selectedText() == QLatin1String("% "))
			textCursor.removeSelectedText();
	}
}

/***************************************************************************/

void TikzEditorView::initReplaceWidgets()
{
	m_replaceWidget = new ReplaceWidget(this);
	m_replaceCurrentWidget = new ReplaceCurrentWidget(this);
	m_replaceCurrentWidget->setVisible(false);

	layout()->addWidget(m_replaceWidget);
	layout()->addWidget(m_replaceCurrentWidget);

	connect(m_replaceWidget, SIGNAL(search(QString,QTextDocument::FindFlags)),
	        this, SLOT(search(QString,QTextDocument::FindFlags)));
	connect(m_replaceWidget, SIGNAL(replace(QString,QString,QTextDocument::FindFlags)),
	        this, SLOT(replace(QString,QString,QTextDocument::FindFlags)));
	connect(m_replaceWidget, SIGNAL(focusEditor()),
	        m_tikzEditor, SLOT(setFocus()));

	connect(m_replaceCurrentWidget, SIGNAL(hidden()),
	        m_tikzEditor, SLOT(setFocus()));
	connect(m_replaceCurrentWidget, SIGNAL(search()),
	        this, SLOT(search()));
	connect(m_replaceCurrentWidget, SIGNAL(replace()),
	        this, SLOT(replace()));
	connect(m_replaceCurrentWidget, SIGNAL(replaceAll()),
	        this, SLOT(replaceAll()));
}

void TikzEditorView::openReplaceWidget()
{
	if (m_goToLineWidget)
		m_goToLineWidget->setVisible(false);
	if (m_indentWidget)
		m_indentWidget->setVisible(false);
	if (!m_replaceWidget)
		initReplaceWidgets();
	m_replaceCurrentWidget->setVisible(false);
	m_replaceWidget->setVisible(true);
	m_replaceWidget->setFocus();
}

void TikzEditorView::editFind()
{
	openReplaceWidget();
	const QTextCursor textCursor = m_tikzEditor->textCursor();
	if (textCursor.hasSelection())
		m_replaceWidget->setText(textCursor.selectedText());
}

bool TikzEditorView::search(const QString &text, QTextDocument::FindFlags flags, bool startAtCursor, bool continueFromBeginning)
{
	bool isFound = false;

	QTextCursor textCursor = m_tikzEditor->textCursor();
	if (!startAtCursor)
	{
		if (!(flags & QTextDocument::FindBackward))
			textCursor.movePosition(QTextCursor::Start);
		else
			textCursor.movePosition(QTextCursor::End);
		m_tikzEditor->setTextCursor(textCursor);
	}
	else
		textCursor.setPosition(textCursor.selectionStart());
	if (!(flags & QTextDocument::FindBackward) && !continueFromBeginning)
		textCursor.movePosition(QTextCursor::Right);
	const QTextCursor found = m_tikzEditor->document()->find(text, textCursor, flags);

	if (found.isNull())
	{
		const QString msg = !(flags & QTextDocument::FindBackward) ?
		                    tr("End of document reached.\n\nContinue from the beginning?")
		                    : tr("Beginning of document reached.\n\nContinue from the end?");
		const int result = MessageBox::questionYesNo(this, msg, tr("Find"), tr("Continue"));
		if (result == MessageBox::Yes)
			return search(text, flags, /*startAtCursor*/ false, /*continueFromBeginning*/ true);
		else
		{
			m_replaceWidget->setVisible(false);
			m_replaceCurrentWidget->setVisible(false);
			m_tikzEditor->setFocus();
		}
	}
	else
	{
		m_tikzEditor->setTextCursor(found);
		isFound = true;
	}
	m_tikzEditor->viewport()->repaint();
	return isFound;
}

void TikzEditorView::search()
{
	search(m_searchText, m_flags, m_startAtCursor);
}

void TikzEditorView::editFindNext()
{
	openReplaceWidget();
	m_replaceWidget->setForward(true);
	m_replaceWidget->doFind();
}

void TikzEditorView::editFindPrevious()
{
	openReplaceWidget();
	m_replaceWidget->setForward(false);
	m_replaceWidget->doFind();
}

void TikzEditorView::editReplace()
{
	editFind();
}

void TikzEditorView::replace(const QString &replacement)
{
	QTextCursor textCursor = m_tikzEditor->textCursor();

	if (textCursor.hasSelection())
	{
		const int start = textCursor.selectionStart();
		textCursor.beginEditBlock();
		textCursor.removeSelectedText();
		textCursor.insertText(replacement);
		textCursor.setPosition(start, QTextCursor::MoveAnchor);
		textCursor.setPosition(start + replacement.length(), QTextCursor::KeepAnchor);
		textCursor.endEditBlock();
		m_tikzEditor->setTextCursor(textCursor);
	}
}

void TikzEditorView::replace(const QString &text, const QString &replacement, QTextDocument::FindFlags flags, bool startAtCursor)
{
	m_searchText = text;
	m_replaceText = replacement;
	m_flags = flags;
	m_startAtCursor = startAtCursor;

	m_replaceWidget->setVisible(false);
	m_replaceCurrentWidget->setReplacement(text, replacement);
	m_replaceCurrentWidget->setVisible(true);
	search(text, flags, startAtCursor);
}

void TikzEditorView::replace()
{
	replace(m_replaceText);
	search(m_searchText, m_flags, m_startAtCursor);
}

void TikzEditorView::replaceAll()
{
	replace(m_replaceText);
	while (search(m_searchText, m_flags, m_startAtCursor))
		replace(m_replaceText);
}

void TikzEditorView::updateCompleter(bool useCompletion, const QStringList &words)
{
	m_tikzEditor->updateCompleter(useCompletion, words);
}
