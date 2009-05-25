/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
 *   glad.deschrijver@gmail.com                                            *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QTextCursor>
#include <QToolBar>

#include "editgotolinewidget.h"
#include "editindentdialog.h"
#include "editreplacewidget.h"
#include "editreplacecurrentwidget.h"
#include "templatewidget.h"
#include "tikzeditor.h"
//#include "tikzeditorhighlighter.h"
#include "tikzeditorview.h"

TikzEditorView::TikzEditorView(QWidget *parent) : QWidget(parent)
{
	m_parentWidget = parent;

	m_templateWidget = new TemplateWidget(this);

	m_tikzEditor = new TikzEditor;
	m_tikzEditor->setWhatsThis("<p>" + tr("Enter your TikZ code here.  "
	    "The code should begin with \\begin{tikzpicture} and end with "
	    "\\end{tikzpicture}.") + "</p>");
/*
	commandInserter = new TikzCommandInserter(tikzEditor, this);
	tikzHighlighter = new TikzHighlighter(commandInserter, tikzEditor->document());
	tikzHighlighter->rehighlight(); // avoid that textEdit emits the signal contentsChanged() when it is still empty
	tikzController = new TikzPngPreviewer(textEdit);
*/

	m_replaceWidget = new ReplaceWidget(this);
	m_replaceWidget->setVisible(false);
	m_replaceCurrentWidget = new ReplaceCurrentWidget(this);
	m_replaceCurrentWidget->setVisible(false);

	m_goToLineWidget = new GoToLineWidget(this);
	m_goToLineWidget->setVisible(false);

//	QFrame *mainWidget = new QFrame;
//	mainWidget->setLineWidth(1);
//	mainWidget->setFrameShape(QFrame::StyledPanel);
//	mainWidget->setFrameShadow(QFrame::Sunken);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	mainLayout->addWidget(m_templateWidget);
	mainLayout->addWidget(m_tikzEditor);
	mainLayout->addWidget(m_replaceWidget);
	mainLayout->addWidget(m_replaceCurrentWidget);
	mainLayout->addWidget(m_goToLineWidget);

	createActions();

	connect(m_templateWidget, SIGNAL(fileNameChanged(QString)),
	        this, SIGNAL(templateFileChanged(QString)));
	connect(m_templateWidget, SIGNAL(focusEditor()),
	        m_tikzEditor, SLOT(setFocus()));
	connect(m_tikzEditor->document(), SIGNAL(modificationChanged(bool)),
	        this, SIGNAL(modificationChanged(bool)));
	connect(m_tikzEditor->document(), SIGNAL(contentsChanged()),
	        this, SIGNAL(contentsChanged()));
	connect(m_tikzEditor, SIGNAL(cursorPositionChanged()),
	        this, SLOT(showCursorPosition()));
	connect(m_replaceWidget, SIGNAL(search(QString,bool,bool,bool)),
	        this, SLOT(search(QString,bool,bool,bool)));
	connect(m_replaceWidget, SIGNAL(replace(QString,QString,bool,bool,bool)),
	        this, SLOT(replace(QString,QString,bool,bool,bool)));
	connect(m_replaceWidget, SIGNAL(focusEditor()),
	        m_tikzEditor, SLOT(setFocus()));
	connect(m_replaceCurrentWidget, SIGNAL(showReplaceWidget()),
	        m_replaceWidget, SLOT(show()));
	connect(m_replaceCurrentWidget, SIGNAL(search(QString,bool,bool,bool,bool)),
	        this, SLOT(search(QString,bool,bool,bool,bool)));
	connect(m_replaceCurrentWidget, SIGNAL(replace(QString)),
	        this, SLOT(replace(QString)));
	connect(m_replaceCurrentWidget, SIGNAL(replaceAll(QString,QString,bool,bool,bool,bool)),
	        this, SLOT(replaceAll(QString,QString,bool,bool,bool,bool)));
	connect(m_replaceCurrentWidget, SIGNAL(setSearchFromBegin(bool)),
	        this, SIGNAL(setSearchFromBegin(bool)));
	connect(m_goToLineWidget, SIGNAL(goToLine(int)),
	        this, SLOT(goToLine(int)));
}

TikzEditorView::~TikzEditorView()
{
//	tikzHighlighter->deleteLater();
}

QTextEdit *TikzEditorView::editor()
{
	return m_tikzEditor;
}

void TikzEditorView::setFont(const QFont &editorFont)
{
	m_tikzEditor->setFont(editorFont);
	m_tikzEditor->setTabStopWidth(m_tikzEditor->fontMetrics().width("    "));
}

void TikzEditorView::setTemplateFile(const QString &fileName)
{
	m_templateWidget->setFileName(fileName);
}

void TikzEditorView::setReplaceText(const QString &replace)
{
	m_templateWidget->setReplaceText(replace);
}

QString TikzEditorView::templateFile() const
{
	return m_templateWidget->fileName();
}

void TikzEditorView::createActions()
{
	const QString undoWhatsThis = tr("Undo the previous action");
	m_undoAction = new QAction(QIcon(":/images/edit-undo.png"), tr("&Undo"), this);
	m_undoAction->setShortcut(QKeySequence::Undo);
	m_undoAction->setStatusTip(undoWhatsThis);
	m_undoAction->setWhatsThis("<p>" + undoWhatsThis + "</p>");
	connect(m_undoAction, SIGNAL(triggered()), m_tikzEditor, SLOT(undo()));

	const QString redoWhatsThis = tr("Redo the previous undone action");
	m_redoAction = new QAction(QIcon(":/images/edit-redo.png"), tr("Re&do"), this);
	m_redoAction->setShortcut(QKeySequence::Redo);
	m_redoAction->setStatusTip(redoWhatsThis);
	m_redoAction->setWhatsThis("<p>" + redoWhatsThis + "</p>");
	connect(m_redoAction, SIGNAL(triggered()), m_tikzEditor, SLOT(redo()));

	const QString cutWhatsThis = tr("Cut the current selection's contents to the clipboard");
	m_cutAction = new QAction(QIcon(":/images/edit-cut.png"), tr("Cu&t"), this);
	m_cutAction->setShortcut(QKeySequence::Cut);
	m_cutAction->setStatusTip(cutWhatsThis);
	m_cutAction->setWhatsThis("<p>" + cutWhatsThis + "</p>");
	connect(m_cutAction, SIGNAL(triggered()), m_tikzEditor, SLOT(cut()));

	const QString copyWhatsThis = tr("Copy the current selection's contents to the clipboard");
	m_copyAction = new QAction(QIcon(":/images/edit-copy.png"), tr("&Copy"), this);
	m_copyAction->setShortcut(QKeySequence::Copy);
	m_copyAction->setStatusTip(copyWhatsThis);
	m_copyAction->setWhatsThis("<p>" + copyWhatsThis + "</p>");
	connect(m_copyAction, SIGNAL(triggered()), m_tikzEditor, SLOT(copy()));

	const QString pasteWhatsThis = tr("Paste the clipboard's contents into the current selection");
	m_pasteAction = new QAction(QIcon(":/images/edit-paste.png"), tr("&Paste"), this);
	m_pasteAction->setShortcut(QKeySequence::Paste);
	m_pasteAction->setStatusTip(pasteWhatsThis);
	m_pasteAction->setWhatsThis("<p>" + pasteWhatsThis + "</p>");
	connect(m_pasteAction, SIGNAL(triggered()), m_tikzEditor, SLOT(paste()));

	m_selectAllAction = new QAction(tr("Select &All"), this);
	m_selectAllAction->setShortcut(QKeySequence::SelectAll);
	m_selectAllAction->setStatusTip(tr("Select all the content"));
	connect(m_selectAllAction, SIGNAL(triggered()), m_tikzEditor, SLOT(selectAll()));

	QAction *action;
	action = new QAction(QIcon(":/images/format-indent-more.png"), tr("&Indent..."), this);
	action->setShortcut(tr("Ctrl+I", "Edit|Indent"));
	action->setStatusTip(tr("Indent the current line or selection"));
	connect(action, SIGNAL(triggered()), this, SLOT(editIndent()));
	m_editActions.append(action);

	action = new QAction(tr("C&omment"), this);
	action->setShortcut(tr("Ctrl+D", "Edit|Comment"));
	action->setStatusTip(tr("Comment the current line or selection"));
	connect(action, SIGNAL(triggered()), this, SLOT(editComment()));
	m_editActions.append(action);

	action = new QAction(tr("Unco&mment"), this);
	action->setShortcut(tr("Ctrl+Shift+D", "Edit|Uncomment"));
	action->setStatusTip(tr("Uncomment the current line or selection"));
	connect(action, SIGNAL(triggered()), this, SLOT(editUncomment()));
	m_editActions.append(action);

	action = new QAction(this);
	action->setSeparator(true);
	m_editActions.append(action);

	action = new QAction(QIcon(":/images/edit-find.png"), tr("&Find..."), this);
	action->setShortcut(QKeySequence::Find);
	action->setStatusTip(tr("Look up a piece of text in the document"));
	connect(action, SIGNAL(triggered()), this, SLOT(editFind()));
	m_editActions.append(action);

	action = new QAction(QIcon(":/images/go-down.png"), tr("Find &Next"), this);
	action->setShortcut(QKeySequence::FindNext);
	action->setStatusTip(tr("Search the next occurrence of a text"));
	connect(action, SIGNAL(triggered()), this, SLOT(editFindNext()));
	m_editActions.append(action);

	action = new QAction(QIcon(":/images/go-up.png"), tr("Find Pre&vious"), this);
	action->setShortcut(QKeySequence::FindPrevious);
	action->setStatusTip(tr("Search the previous occurrence of a text"));
	connect(action, SIGNAL(triggered()), this, SLOT(editFindPrevious()));
	m_editActions.append(action);

	action = new QAction(tr("&Replace..."), this);
	action->setShortcut(QKeySequence::Replace);
	action->setStatusTip(tr("Search and replace a piece of text in the document"));
	connect(action, SIGNAL(triggered()), this, SLOT(editReplace()));
	m_editActions.append(action);

	action = new QAction(QIcon(":/images/go-jump.png"), tr("&Go to Line..."), this);
//	action->setShortcut(Qt::CTRL+Qt::Key_G);
	action->setShortcut(tr("Ctrl+G", "Edit|Go to Line"));
	action->setStatusTip(tr("Go to a certain line in the document"));
	connect(action, SIGNAL(triggered()), this, SLOT(editGoToLine()));
	m_editActions.append(action);

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
}

QMenu *TikzEditorView::createMenu()
{
	QMenu *editMenu = new QMenu(tr("&Edit"), m_parentWidget);
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

QToolBar *TikzEditorView::createToolBar()
{
	QToolBar *editToolBar = new QToolBar(tr("Edit"), m_parentWidget);
	editToolBar->setObjectName("EditToolBar");
	editToolBar->addAction(m_undoAction);
	editToolBar->addAction(m_redoAction);
//	editToolBar->addAction(m_cutAction);
//	editToolBar->addAction(m_copyAction);
//	editToolBar->addAction(m_pasteAction);
	return editToolBar;
}

void TikzEditorView::setPasteEnabled()
{
	m_pasteAction->setEnabled(m_tikzEditor->canPaste());
}

void TikzEditorView::applySettings()
{
	QSettings settings;

	m_templateWidget->setEditor(settings.value("TemplateEditor", "kwrite").toString());

	settings.beginGroup("Editor");
	QFont editorFont;
	editorFont.fromString(settings.value("Font", qApp->font().toString()).toString());
	setFont(editorFont);
	m_tikzEditor->setShowMatchingBrackets(settings.value("ShowMatchingBrackets", true).toBool());
	m_tikzEditor->setShowWhiteSpaces(settings.value("ShowWhiteSpaces", false).toBool());
	m_tikzEditor->setMatchingColor(settings.value("ColorMatchingBrackets", Qt::yellow).value<QColor>());
	settings.endGroup();

/*
	settings.beginGroup("Highlighting");
	customHighlighting = settings.value("Customize", true).toBool();
	QMap<QString, QTextCharFormat> formatList = tikzHighlighter->getDefaultHighlightFormats();
	if (customHighlighting)
	{
		int numOfRules = settings.value("Number", 0).toInt();
		for (int i = 0; i < numOfRules; ++i)
		{
			QString name = settings.value("Item" + QString::number(i) + "/Name").toString();
			QString colorName = settings.value("Item" + QString::number(i) + "/Color").toString();
			QString fontName = settings.value("Item" + QString::number(i) + "/Font").toString();
			QFont font;
			font.fromString(fontName);
			QTextCharFormat format;
			format.setForeground(QBrush(QColor(colorName)));
			format.setFont(font);
			formatList[name] = format;
		}
	}
	settings.endGroup();

	tikzHighlighter->setTextCharFormats(formatList);
	tikzHighlighter->rehighlight();
*/
}

/***************************************************************************/

void TikzEditorView::showCursorPosition()
{
	QTextCursor cur = m_tikzEditor->textCursor();
	const int curPos = cur.position();
	cur.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
	emit cursorPositionChanged(cur.blockNumber() + 1, curPos - cur.position() + 1);
}

/***************************************************************************/
/* Go to line */

void TikzEditorView::setLine(const QString &line)
{
	bool ok;
	const int lineNumber = line.toInt(&ok, 10);
	if (ok) setLine(lineNumber);
}


void TikzEditorView::setLine(int lineNumber)
{
	m_replaceWidget->setVisible(false);
	m_goToLineWidget->setVisible(true);
	m_goToLineWidget->setFocus();
	m_goToLineWidget->setMaximumValue(m_tikzEditor->numOfLines());
	m_goToLineWidget->setValue(lineNumber);
}

void TikzEditorView::goToLine(int lineNumber)
{
	m_goToLineWidget->setVisible(false);
	m_tikzEditor->goToLine(lineNumber);
	m_tikzEditor->setFocus();
}

void TikzEditorView::editGoToLine()
{
	int lineNumber;
	lineNumber = m_tikzEditor->textCursor().blockNumber() + 1;
	if (lineNumber < 1)
		lineNumber = 1;
	setLine(lineNumber);
}

/***************************************************************************/

void TikzEditorView::editIndent()
{
	IndentDialog *indentDialog = new IndentDialog(this, "Indent");
	if (!indentDialog->exec())
	{
		delete indentDialog;
		return;
	}

	const QString insertString = indentDialog->insertChar();
	const int numOfInserts = indentDialog->numOfInserts();
	delete indentDialog;

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
			for (int i = 0; i < numOfInserts; ++i)
				textCursor.insertText(insertString);
			end++;
			go = textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
		}
		textCursor.endEditBlock();
	}
	else
	{
		textCursor.beginEditBlock();
		textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		for (int i = 0; i < numOfInserts; ++i)
			textCursor.insertText(insertString);
		textCursor.endEditBlock();
	}
}

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
			if (textCursor.selectedText() == "% ")
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
		if (textCursor.selectedText() == "% ")
			textCursor.removeSelectedText();
	}
}

/***************************************************************************/

void TikzEditorView::editFind()
{
	m_goToLineWidget->setVisible(false);
	m_replaceWidget->setVisible(true);
	m_replaceWidget->setFocus();
	const QTextCursor textCursor = m_tikzEditor->textCursor();
	if (textCursor.hasSelection())
		m_replaceWidget->setText(textCursor.selectedText());
}

bool TikzEditorView::search(const QString &text, bool isCaseSensitive,
    bool findWholeWords, bool forward, bool startAtCursor)
{
	bool isFound = false;

	QTextDocument::FindFlags flags = 0;
	if (isCaseSensitive) flags |= QTextDocument::FindCaseSensitively;
	if (findWholeWords) flags |= QTextDocument::FindWholeWords;

	QTextCursor textCursor = m_tikzEditor->textCursor();
	if (!startAtCursor)
	{
		if (forward) textCursor.movePosition(QTextCursor::Start);
		else textCursor.movePosition(QTextCursor::End);
		m_tikzEditor->setTextCursor(textCursor);
	}
	else
		textCursor.setPosition(textCursor.selectionStart());
	if (!forward) flags |= QTextDocument::FindBackward;
	else textCursor.movePosition(QTextCursor::Right);
	const QTextCursor found = m_tikzEditor->document()->find(text, textCursor, flags);

	if (found.isNull())
	{
		const QString msg = (forward) ?
		    tr("End of document reached.\n\nContinue from the beginning?")
		    : tr("Beginning of document reached.\n\nContinue from the end?");
		const int ret = QMessageBox::warning(this, "Find", msg,
		    QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape);
		if (ret == QMessageBox::Yes)
		{
			return search(text, isCaseSensitive, findWholeWords, forward, false);
		}
	}
	else
	{
		m_tikzEditor->setTextCursor(found);
		emit setSearchFromBegin(false);
		isFound = true;
	}
	m_tikzEditor->viewport()->repaint();
	return isFound;
}

void TikzEditorView::editFindNext()
{
	m_goToLineWidget->setVisible(false);
	m_replaceWidget->setVisible(true);
	m_replaceWidget->setFocus();
	m_replaceWidget->setForward(true);
	m_replaceWidget->doFind();
}

void TikzEditorView::editFindPrevious()
{
	m_goToLineWidget->setVisible(false);
	m_replaceWidget->setVisible(true);
	m_replaceWidget->setFocus();
	m_replaceWidget->setForward(false);
	m_replaceWidget->doFind();
}

void TikzEditorView::editReplace()
{
/*
	m_goToLineWidget->setVisible(false);
	m_replaceWidget->setVisible(true);
	m_replaceWidget->setFocus();
	const QTextCursor textCursor = m_tikzEditor->textCursor();
	if (textCursor.hasSelection())
		m_replaceWidget->setText(textCursor.selectedText());
*/
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

void TikzEditorView::replace(const QString &text, const QString &replacement,
    bool isCaseSensitive, bool findWholeWords, bool forward, bool startAtCursor)
{
	m_replaceWidget->setVisible(false);
	m_replaceCurrentWidget->setReplacement(text, replacement);
	m_replaceCurrentWidget->setVisible(true);
	m_replaceCurrentWidget->search(text, replacement, isCaseSensitive, findWholeWords, forward, startAtCursor);
/*
	bool go = true;
	while (go && search(text, isCaseSensitive, findWholeWords, forward, startAtCursor))
	{
		switch(QMessageBox::warning(this, "Replace", tr("Replace this occurence?"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes))
		{
			case QMessageBox::Yes:
				replace(replacement);
				emit setSearchFromBegin(false);
				break;
			case QMessageBox::YesToAll:
				replace(replacement);
				replaceAll(text, replacement, isCaseSensitive, findWholeWords, forward, startAtCursor);
				return;
				break;
			case QMessageBox::No:
				emit setSearchFromBegin(false);
				break;
			case QMessageBox::Cancel:
				go = false;
				break;
			default:
				break;
		}
	}
	if (go)
		emit setSearchFromBegin(true);
*/
}

void TikzEditorView::replaceAll(const QString &text, const QString &replacement,
    bool isCaseSensitive, bool findWholeWords, bool forward, bool startAtCursor)
{
	while (search(text, isCaseSensitive, findWholeWords, forward, startAtCursor))
	{
		replace(replacement);
		emit setSearchFromBegin(false);
	}
	emit setSearchFromBegin(true);
}

void TikzEditorView::setCompleter(QCompleter *completer)
{
	m_tikzEditor->setCompleter(completer);
}
