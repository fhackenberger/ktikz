/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011 by Glad Deschrijver        *
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

#include "tikzcommandinserter.h"

#include <QtGui/QApplication>
#include <QtGui/QDockWidget>
#include <QtCore/QFile>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMenu>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QStackedWidget>
#include <QtGui/QTextCursor>

#include "tikzeditorhighlighter.h"
#include "tikzcommandwidget.h"
#include "../common/utils/combobox.h"

static const QString s_completionPlaceHolder(0x2022);

TikzCommandInserter::TikzCommandInserter(QWidget *parent)
	: QObject(parent)
{
	m_parentWidget = parent;

	getCommands();

	highlightTypeNames << "Commands" << "Draw to" << "Options";
}

void TikzCommandInserter::getCommands()
{
	QFile tagsFile(":/tikzcommands.xml");
	if (!tagsFile.open(QFile::ReadOnly))
		return;

	QApplication::setOverrideCursor(Qt::WaitCursor);
	xml.setDevice(&tagsFile);
	if (xml.readNextStartElement())
	{
		if (xml.name() == "tikzcommands")
			m_tikzSections = getChildCommands();
		else
			xml.raiseError(tr("Cannot parse the TikZ commands file."));
	}
	if (xml.error()) // this should never happen in a final release because tikzcommands.xml is built in the binary
		qCritical("Parse error in TikZ commands file at line %d, column %d:\n%s", (int)xml.lineNumber(), (int)xml.columnNumber(), qPrintable(xml.errorString()));
	QApplication::restoreOverrideCursor();
}

TikzCommandList TikzCommandInserter::getChildCommands()
{
	TikzCommandList commandList;
	QList<TikzCommand> commands;

	commandList.title = tr(xml.attributes().value("title").toString().toLatin1().data());

	QString name;
	QString description;
	QString insertion;
	QString highlightString;
	QString type;
	QRegExp newLineRegExp("([^\\\\])\\\\n"); // newlines are the "\n" not preceded by a backslash as in "\\node"
	QRegExp descriptionRegExp("<([^<>]*)>"); // descriptions are between < and >

	while (xml.readNextStartElement())
	{
		if (xml.name() == "item")
		{
			name = tr(xml.attributes().value("name").toString().toLatin1().data());
			description = xml.attributes().value("description").toString();
			insertion = xml.attributes().value("insert").toString();
			highlightString = xml.attributes().value("highlight").toString();
			type = xml.attributes().value("type").toString();

			if (description.contains(QLatin1String("\\n"))) // minimize the number of uses of QRegExp
			{
				description.replace(newLineRegExp, QLatin1String("\\1\n")); // replace newlines, these are the "\n" not preceded by a backslash as in "\\node"
				description.replace(newLineRegExp, QLatin1String("\\1\n")); // do this twice to replace all newlines
			}
			description.replace(QLatin1String("\\\\"), QLatin1String("\\"));
			// translate options in the description:
			QString tempDescription;
			for (int pos = 0, oldPos = 0; pos >= 0;)
			{
				oldPos = pos;
				pos = descriptionRegExp.indexIn(description, pos);
				tempDescription += description.midRef(oldPos, pos - oldPos + 1);
				if (pos >= 0)
				{
					tempDescription += tr(descriptionRegExp.cap(1).toLatin1().data());
					pos += descriptionRegExp.matchedLength() - 1;
				}
			}
			if (!tempDescription.isEmpty())
				description = tempDescription;

			if (insertion.contains(QLatin1String("\\n"))) // minimize the number of uses of QRegExp
			{
				insertion.replace(newLineRegExp, QLatin1String("\\1\n")); // replace newlines, these are the "\n" not preceded by a backslash as in "\\node"
				insertion.replace(newLineRegExp, QLatin1String("\\1\n")); // do this twice to replace all newlines
			}
			insertion.replace(QLatin1String("\\\\"), QLatin1String("\\"));

			if (name.isEmpty())
			{
				name = description;
				description.remove('&'); // we assume that if name.isEmpty() then an accelerator is defined in description
				if (name.isEmpty())
					name = insertion;
			}
			if (description.isEmpty())
				description = insertion;
			if (type.isEmpty())
				type = '0';

			commands << newCommand(name, description, insertion, highlightString, xml.attributes().value("dx").toString().toInt(), xml.attributes().value("dy").toString().toInt(), type.toInt());
			xml.skipCurrentElement(); // allow to read the next start element on the same level: this skips reading the current end element which would cause xml.readNextStartElement() to evaluate to false
		}
		else if (xml.name() == "separator")
		{
			commands << newCommand("", "", "", "", 0, 0, 0);
			xml.skipCurrentElement(); // same as above
		}
		else if (xml.name() == "section")
		{
			commands << newCommand("", "", "", "", 0, 0, -1); // the i-th command with type == -1 corresponds to the i-th submenu (assumed in getMenu())
			commandList.children << getChildCommands();
		}
		else
			xml.skipCurrentElement();
	}
	commandList.commands = commands;

	return commandList;
}

QStringList TikzCommandInserter::getCommandWords()
{
	QStringList words;
	QString word;

	QRegExp rx1("^([^a-z\\\\<>]*<[^>]*>)*");
	QRegExp rx2("^[^a-z\\\\]*");
	QString allowedLetters = "abcdefghijklmnopqrstuvwxyz\\";
	for (int i = 0; i < m_tikzCommandsList.size(); ++i)
	{
		word = m_tikzCommandsList.at(i).description;
		// remove all special characters and <options> at the beginning of the word
		if (!word.isEmpty() && !allowedLetters.contains(word.at(0))) // minimize the number of uses of QRegExp
		{
			word.remove(rx1);
			word.remove(rx2);
		}
		if (!word.isEmpty())
			words.append(word);
		else
		{
			word = m_tikzCommandsList.at(i).command;
			// remove all special characters and <options> at the beginning of the word
			if (!word.isEmpty() && !allowedLetters.contains(word.at(0))) // minimize the number of uses of QRegExp
			{
				word.remove(rx1);
				word.remove(rx2);
			}
			if (!word.isEmpty())
				words.append(word);
		}
	}

	return words;
}

/*!
 * \name Menu with TikZ commands
 */
//@{

void TikzCommandInserter::updateDescriptionMenuItem()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
	{
		const int num = action->data().toInt();
		const TikzCommand cmd = m_tikzCommandsList.at(num);
		QList<QAction*> menuActions = qobject_cast<QMenu*>(action->parentWidget())->actions();
		QString description = cmd.description;
		description.replace(QLatin1Char('&'), QLatin1String("&&")); // don't use ampersands in the description to create a keyboard accelerator
		menuActions[menuActions.size()-1]->setText(description);
		menuActions[menuActions.size()-1]->setData(cmd.number);
	}
}

QMenu *TikzCommandInserter::getMenu(const TikzCommandList &commandList)
{
	QMenu *menu = new QMenu(commandList.title, m_parentWidget);
	const int numOfCommands = commandList.commands.size();
	QAction *action = new QAction("test", menu);
	int whichSection = 0;

	// get left margin of the menu (to be added to the minimum width of the menu)
	menu->addAction(action);
	QFont actionFont = action->font();
	actionFont.setPointSize(actionFont.pointSize() - 1);
	QFontMetrics actionFontMetrics(actionFont);
	int menuLeftMargin = menu->width() - actionFontMetrics.boundingRect(action->text()).width();
	menu->removeAction(action);
	int menuMinimumWidth = 0;

	for (int i = 0; i < numOfCommands; ++i)
	{
		const QString name = commandList.commands.at(i).name;
		if (name.isEmpty()) // add separator or submenu
		{
			if (commandList.commands.at(i).type == 0)
			{
				action = new QAction(menu);
				action->setSeparator(true);
				menu->addAction(action);
			}
			else // type == -1, so add submenu; this assumes that the i-th command with type == -1 corresponds with the i-th submenu (see getCommands())
			{
				menu->addMenu(getMenu(commandList.children.at(whichSection)));
				++whichSection;
			}
		}
		else // add command
		{
			action = new QAction(name, menu);
			action->setData(commandList.commands.at(i).number); // link to the corresponding item in m_tikzCommandsList
			action->setStatusTip(commandList.commands.at(i).description);
			menuMinimumWidth = qMax(menuMinimumWidth, actionFontMetrics.boundingRect(commandList.commands.at(i).description).width());
			connect(action, SIGNAL(triggered()), this, SLOT(insertTag()));
			connect(action, SIGNAL(hovered()), this, SLOT(updateDescriptionMenuItem()));
			menu->addAction(action);
		}
	}

	// if the menu does not only contain submenus, then we add a menu item
	// at the bottom of the menu which shows the description of the currently
	// highlighted menu item
	if (whichSection < menu->actions().size())
	{
		action = new QAction(this);
		action->setSeparator(true);
		menu->addAction(action);

		action = new QAction(this);
		QFont actionFont = action->font();
		actionFont.setPointSize(actionFont.pointSize() - 1);
		action->setFont(actionFont);
		connect(action, SIGNAL(triggered()), this, SLOT(insertTag()));
		menu->addAction(action);

		// make sure that the menu width does not change when the content
		// of the above menu item changes
		menu->setMinimumWidth(menuMinimumWidth + menuLeftMargin);
	}

	return menu;
}

/*!
 * This function returns a menu containing a list of TikZ commands
 * which can be inserted in the main text.
 * \return a menu with TikZ commands
 */

QMenu *TikzCommandInserter::getMenu()
{
	return getMenu(m_tikzSections);
}

//@}

/*!
 * \name Dock widget with TikZ commands
 */
//@{

void TikzCommandInserter::addListWidgetItems(QListWidget *listWidget, const TikzCommandList &commandList, bool addChildren)
{
	QFont titleFont = qApp->font();
	titleFont.setBold(true);
//	QColor titleBg(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Highlight));
//	titleBg = titleBg.lighter(120);
	QColor titleBg(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Window));
	titleBg = titleBg.darker(200);
	QColor titleFg(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::HighlightedText));
	QColor separatorBg(QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::AlternateBase));
	if (separatorBg == QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Base))
		separatorBg = separatorBg.darker(110);

	for (int i = 0; i < commandList.commands.size(); ++i)
	{
		if (commandList.commands.at(i).type == -1) // if we have an empty command corresponding to a submenu, then don't add the command, the submenus will be added later
			continue;

		QListWidgetItem *item = new QListWidgetItem(listWidget);
		QString itemText = commandList.commands.at(i).name;
		item->setText(itemText.remove('&'));

		if (itemText.isEmpty())
			item->setBackgroundColor(separatorBg);
		else
			item->setData(Qt::UserRole, commandList.commands.at(i).number); // link to the corresponding item in m_tikzCommandsList
	}

	if (!addChildren) return;

	for (int i = 0; i < commandList.children.size(); ++i)
	{
		QListWidgetItem *item = new QListWidgetItem(listWidget);
		QString itemText = commandList.children.at(i).title;
		item->setText(itemText.remove('&'));

		item->setBackgroundColor(titleBg);
		item->setTextColor(titleFg);
		item->setFont(titleFont);

		addListWidgetItems(listWidget, commandList.children.at(i));
	}
}

/*!
 * This function returns a dock widget containing a list of TikZ commands
 * which can be inserted in the main text by clicking on them.
 * \param parent the parent widget
 * \return a dock widget with TikZ commands
 */

QDockWidget *TikzCommandInserter::getDockWidget(QWidget *parent)
{
	QDockWidget *tikzDock = new QDockWidget(parent);
	tikzDock->setObjectName("CommandsDock");
	tikzDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	tikzDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	tikzDock->setWindowTitle(m_tikzSections.title);
	tikzDock->setWhatsThis(tr("<p>This is a list of TikZ "
	                          "commands.  You can insert these commands in your code by "
	                          "clicking on them.  You can obtain more commands by "
	                          "changing the category in the combo box.</p>"));

	QAction *focusTikzDockAction = new QAction(parent);
	focusTikzDockAction->setShortcut(QKeySequence(tr("Alt+I")));
	tikzDock->addAction(focusTikzDockAction);
	connect(focusTikzDockAction, SIGNAL(triggered()), tikzDock, SLOT(setFocus()));

	QLabel *commandsComboLabel = new QLabel(tr("Category:"));
	ComboBox *commandsCombo = new ComboBox;
	commandsCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QStackedWidget *commandsStack = new QStackedWidget;
	connect(commandsCombo, SIGNAL(currentIndexChanged(int)), commandsStack, SLOT(setCurrentIndex(int)));

	QListWidget *tikzListWidget = new QListWidget;
	addListWidgetItems(tikzListWidget, m_tikzSections, false); // don't add children
	tikzListWidget->setMouseTracking(true);
	connect(tikzListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
	connect(tikzListWidget, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
	connect(tikzListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));
//	connect(tikzListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));
	commandsCombo->addItem(tr("General"));
	commandsStack->addWidget(tikzListWidget);

	for (int i = 0; i < m_tikzSections.children.size(); ++i)
	{
		QListWidget *tikzListWidget = new QListWidget;
		addListWidgetItems(tikzListWidget, m_tikzSections.children.at(i));
		tikzListWidget->setMouseTracking(true);
		connect(tikzListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
		connect(tikzListWidget, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
		connect(tikzListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));
//		connect(tikzListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));

		QString comboItemText = m_tikzSections.children.at(i).title;
		commandsCombo->addItem(comboItemText.remove('&'));
		commandsStack->addWidget(tikzListWidget);
	}

	QGridLayout *tikzLayout = new QGridLayout;
	tikzLayout->addWidget(commandsComboLabel, 0, 0);
	tikzLayout->addWidget(commandsCombo, 0, 1);
	tikzLayout->addWidget(commandsStack, 1, 0, 1, 2);
	tikzLayout->setMargin(5);

	TikzCommandWidget *tikzWidget = new TikzCommandWidget;
	tikzWidget->setLayout(tikzLayout);
	tikzDock->setWidget(tikzWidget);
	tikzDock->setFocusProxy(commandsCombo);

	return tikzDock;
}

void TikzCommandInserter::setEditor(QPlainTextEdit *textEdit)
{
	m_mainEdit = textEdit;
}

void TikzCommandInserter::setListStatusTip(QListWidgetItem *item)
{
	if (item && !item->font().bold() && !item->text().isEmpty())
	{
		const int num = item->data(Qt::UserRole).toInt();
		emit showStatusMessage(m_tikzCommandsList.at(num).description, 10000);
	}
	else
		emit showStatusMessage("");
}

//@}

/*!
 * Returns a list of default formats which will be used by TikzHighlighter
 * to highlight the different types of highlighting rules given by
 * getHighlightTypeNames().  Any value in the list returned by
 * getHighlightTypeNames() serves as a key in this list.
 * \return a list of default formats
 */

QMap<QString, QTextCharFormat> TikzCommandInserter::getDefaultHighlightFormats()
{
	QMap<QString, QTextCharFormat> formatList;

	QTextCharFormat commandFormat;
	commandFormat.setForeground(QColor("#004080"));
	commandFormat.setFont(qApp->font());
	commandFormat.setFontWeight(QFont::Bold);
	formatList[highlightTypeNames.at(0)] = commandFormat;

	QTextCharFormat drawFormat;
	drawFormat.setForeground(Qt::darkRed);
	drawFormat.setFont(qApp->font());
	drawFormat.setFontWeight(QFont::Normal);
	formatList[highlightTypeNames.at(1)] = drawFormat;

	QTextCharFormat optionFormat;
	optionFormat.setForeground(QColor("#004000"));
	optionFormat.setFont(qApp->font());
	optionFormat.setFontWeight(QFont::Normal);
	formatList[highlightTypeNames.at(2)] = optionFormat;

	return formatList;
}

/*!
 * Returns the list of the names of the different types of highlighting
 * rules provided by this class.  This list contains the names which may
 * be presented in a user interface and which will be translated when
 * another language is used.
 * \return a list of types of highlighting rules
 * \see getHighlightTypeNames
 */

QStringList TikzCommandInserter::getTranslatedHighlightTypeNames()
{
	QStringList translatedHighlightTypeNames;
	translatedHighlightTypeNames << tr("Commands") << tr("Draw to") << tr("Options");
	return translatedHighlightTypeNames;
}

/*!
 * Returns the list of the names of the different types of highlighting
 * rules provided by this class.  This list contains the untranslated
 * versions of the type names and should not be used in the user interface.
 * \return a list of types of highlighting rules
 * \see getTranslatedHighlightTypeNames
 */

QStringList TikzCommandInserter::getHighlightTypeNames()
{
	return highlightTypeNames;
}

/*!
 * Returns a vector with the highlighting rules derived from the TikZ
 * commands which are available in the menu/dock widget.  These rules
 * are used by TikzHighlighter to highlight the commands in the text.
 * \return a vector containing the highlighting rules
 */

QVector<HighlightingRule> TikzCommandInserter::getHighlightingRules()
{
	QVector<HighlightingRule> highlightingRules;
	HighlightingRule rule;

	for (int i = 0; i < m_tikzCommandsList.size(); ++i)
	{
		QString command = m_tikzCommandsList.at(i).command;
		const int type = m_tikzCommandsList.at(i).type;
		int end;
		rule.pattern = QRegExp(m_tikzCommandsList.at(i).highlightString);
		switch (type)
		{
			case 1:
			{
				const int end1 = command.indexOf(' ', 0);
				const int end2 = command.indexOf('[', 0);
				const int end3 = command.indexOf('{', 0);
				end = end1;
				if (end < 0 || (end2 >= 0 && end2 < end))
					end = end2;
				if (end < 0 || (end3 >= 0 && end3 < end))
					end = end3;

				command = command.left(end);
//				command = command.replace(QLatin1Char('\\'), QLatin1String("\\\\"));
				rule.type = highlightTypeNames.at(0);
//				rule.pattern = QRegExp(command);
//				rule.pattern.setPattern(command);
				rule.matchString = command;
				highlightingRules.append(rule);
				break;
			}
			case 2:
//				command = command.replace("()", "\\([^\\)]*\\)");
//				command = command.replace("(,)", "\\([^\\)]*\\)");
//				command = command.replace("(:::)", "\\([^\\)]*\\)");
				command = command.remove('+');
				command = command.remove(" ()");
				command = command.remove(" (,)");
				command = command.remove(" (:::)");
				command = command.remove(" {} ");
				rule.type = highlightTypeNames.at(1);
//				rule.pattern = QRegExp(command);
//				rule.pattern.setPattern(command);
				rule.matchString = command;
				highlightingRules.append(rule);
				break;
			case 3:
//				command = command.replace(QLatin1Char('|'), QLatin1String("\\|"));
				end = command.indexOf('=', 0) + 1;
				if (end > 0)
					command = command.left(end);
				rule.type = highlightTypeNames.at(2);
//				rule.pattern = QRegExp(command);
//				rule.pattern.setPattern(command);
				rule.matchString = command;
				highlightingRules.append(rule);
				break;
		}
	}

	return highlightingRules;
}

TikzCommand TikzCommandInserter::newCommand(const QString &name,
        const QString &command, int dx, int dy, int type)
{
	return newCommand(name, "", command, "", dx, dy, type);
}

TikzCommand TikzCommandInserter::newCommand(const QString &name,
        const QString &description, const QString &command,
        const QString &highlightString, int dx, int dy, int type)
{
	/* type:
	 * 0: plain text
	 * 1: command
	 * 2: draw to next point
	 * 3: option */
	TikzCommand tikzCommand;
	tikzCommand.name = name;
	tikzCommand.description = description;
	tikzCommand.command = command;
	tikzCommand.highlightString = highlightString;
	tikzCommand.dx = dx;
	tikzCommand.dy = dy;
	tikzCommand.type = type;
	tikzCommand.number = m_tikzCommandsList.size();
	m_tikzCommandsList << tikzCommand;

	return tikzCommand;
}

void TikzCommandInserter::insertTag()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
	{
		const int num = action->data().toInt();
		const TikzCommand cmd = m_tikzCommandsList.at(num);
		emit showStatusMessage(cmd.description, 0);
		insertTag(cmd.command, cmd.dx, cmd.dy);
	}
}

void TikzCommandInserter::insertTag(QListWidgetItem *item)
{
	if (item && !item->font().bold() && !item->text().isEmpty())
	{
		const int num = item->data(Qt::UserRole).toInt();
		const TikzCommand cmd = m_tikzCommandsList.at(num);
		emit showStatusMessage(cmd.description, 0);
		insertTag(cmd.command, cmd.dx, cmd.dy);
	}
}

/*!
 * Inserts a TikZ command and moves the cursor to a position located
 * dx characters to the right of and dy characters below the
 * start of the inserted tag.
 * \param tag the TikZ command to be inserted
 * \param dx the x-offset of the new cursor position w.r.t. the beginning of the inserted tag
 * \param dy the y-offset of the new cursor position w.r.t. the beginning of the inserted tag
 */

void TikzCommandInserter::insertTag(const QString &tag, int dx, int dy)
{
	QTextCursor cur = m_mainEdit->textCursor();
	const int pos = cur.position();
	m_mainEdit->insertPlainText(tag);
	cur.setPosition(pos, QTextCursor::MoveAnchor);
	if (tag.contains(s_completionPlaceHolder))
	{
		cur = m_mainEdit->document()->find(s_completionPlaceHolder, cur);
		m_mainEdit->setTextCursor(cur);
	}
	else if (dx > 0 || dy > 0)
	{
		if (dy > 0)
		{
			cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, dy);
			cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor, 1);
		}
		if (dx > 0)
			cur.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, dx);
		m_mainEdit->setTextCursor(cur);
	}
	// else we are only inserting a string with no placeholders and no positioning, so the cursor must come at the end of the string (this is done automatically by Qt)

	m_mainEdit->setFocus();
}
