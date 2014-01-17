/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013                *
 *   by Glad Deschrijver <glad.deschrijver@gmail.com>                      *
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

#include <QtCore/QFile>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextCursor>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStackedWidget>
#else
#include <QtGui/QApplication>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMenu>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QStackedWidget>
#endif

#include "tikzeditorhighlighter.h"
#include "tikzcommandwidget.h"
#include "../common/utils/combobox.h"

static const QString s_completionPlaceHolder(0x2022);

TikzCommandList TikzCommandInserter::m_tikzSections;
QList<TikzCommand> TikzCommandInserter::m_tikzCommandsList;

TikzCommandInserter::TikzCommandInserter(QWidget *parent)
	: QObject(parent)
{
}

/***************************************************************************/

static TikzCommand newCommand(const QString &name,
        const QString &description, const QString &command,
        const QString &highlightString, int dx, int dy, int type)
{
	// type:
	//   0: plain text
	//   1: command
	//   2: draw to next point
	//   3: option
	TikzCommand tikzCommand;
	tikzCommand.name = name;
	tikzCommand.description = description;
	tikzCommand.command = command;
	tikzCommand.highlightString = highlightString;
	tikzCommand.dx = dx;
	tikzCommand.dy = dy;
	tikzCommand.type = type;

	return tikzCommand;
}

static QString translateOptions(const QString &text)
{
	QString translatedText;
	for (int pos = 0, oldPos = 0; pos >= 0;)
	{
		oldPos = pos;
		pos = text.indexOf(QLatin1Char('<'), pos); // option is between < and >
		translatedText += text.midRef(oldPos, pos - oldPos + 1); // add text between the current option and the previous option; this also adds the end of the original string, except when there are no options
		if (pos >= 0)
		{
			oldPos = pos;
			pos = text.indexOf(QLatin1Char('>'), pos); // option is between < and >
			translatedText += QCoreApplication::translate("TikzCommandInserter", text.mid(oldPos+1, pos - oldPos - 1).toLatin1().data());
		}
	}
	if (!translatedText.isEmpty()) // when there are no options, translatedText is empty
		return translatedText;
	return text;
}

static QString restoreNewLines(const QString &text)
{
	QString newText = text;
	// replace all "\n" not preceded by a backslash (as in "\\node") by a newline character
	for (int pos = 0; ; ++pos)
	{
		pos = newText.indexOf(QLatin1String("\\n"), pos);
		if (pos < 0)
			break;
		if (pos == 0 || newText.at(pos-1) != QLatin1Char('\\'))
			newText.replace(pos, 2, QLatin1Char('\n'));
	}
	return newText;
}

static TikzCommandList getChildCommands(QXmlStreamReader *xml, QList<TikzCommand> *tikzCommandsList)
{
	TikzCommandList commandList;
	QList<TikzCommand> commands;

	commandList.title = QApplication::translate("TikzCommandInserter", xml->attributes().value("title").toString().toLatin1().data());

	QString name;
	QString description;
	QString insertion;
	QString highlightString;
	QString type;

	while (xml->readNextStartElement())
	{
		if (xml->name() == "item")
		{
			QXmlStreamAttributes xmlAttributes = xml->attributes();
			name = QApplication::translate("TikzCommandInserter", xmlAttributes.value("name").toString().toLatin1().data());
			description = xmlAttributes.value("description").toString();
			insertion = xmlAttributes.value("insert").toString();
			highlightString = xmlAttributes.value("highlight").toString();
			type = xmlAttributes.value("type").toString();

			// currently description contains no newlines, otherwise add code to replace all "\n" not preceded by a backslash (as in "\\node") by a newline character
			description.replace(QLatin1String("\\\\"), QLatin1String("\\"));
			description = translateOptions(description);

			insertion = restoreNewLines(insertion); // this must be done before the next line
			insertion.replace(QLatin1String("\\\\"), QLatin1String("\\"));

			if (description.isEmpty()) // if both name and description are empty, setting the description first ensures that name is also set to insertion
				description = insertion;
			if (name.isEmpty())
			{
				name = description;
				description.remove('&'); // we assume that if name.isEmpty() then an accelerator is defined in description
			}
			if (type.isEmpty())
				type = '0';

			TikzCommand tikzCommand = newCommand(name, description, insertion, highlightString, xmlAttributes.value("dx").toString().toInt(), xmlAttributes.value("dy").toString().toInt(), type.toInt());
			tikzCommand.number = tikzCommandsList->size();
			tikzCommandsList->append(tikzCommand);
			commands << tikzCommand;
			xml->skipCurrentElement(); // allow to read the next start element on the same level: this skips reading the current end element which would cause xml.readNextStartElement() to evaluate to false
		}
		else if (xml->name() == "separator")
		{
			commands << newCommand("", "", "", "", 0, 0, 0);
			xml->skipCurrentElement(); // same as above
		}
		else if (xml->name() == "section")
		{
			commands << newCommand("", "", "", "", 0, 0, -1); // the i-th command with type == -1 corresponds to the i-th submenu (assumed in getMenu())
			commandList.children << getChildCommands(xml, tikzCommandsList);
		}
		else
			xml->skipCurrentElement();
	}
	commandList.commands = commands;

	return commandList;
}

static TikzCommandList getCommands(QXmlStreamReader *xml, QList<TikzCommand> *tikzCommandsList)
{
	TikzCommandList commandList;

	QFile tagsFile(":/tikzcommands.xml");
	if (!tagsFile.open(QFile::ReadOnly))
		return commandList;

	xml->setDevice(&tagsFile);
	if (xml->readNextStartElement())
	{
		if (xml->name() == "tikzcommands")
			commandList = getChildCommands(xml, tikzCommandsList);
		else
			xml->raiseError(QApplication::translate("TikzCommandInserter", "Cannot parse the TikZ commands file."));
	}
	if (xml->error()) // this should never happen in a final release because tikzcommands.xml is built in the binary
		qCritical("Parse error in TikZ commands file at line %d, column %d:\n%s", (int)xml->lineNumber(), (int)xml->columnNumber(), qPrintable(xml->errorString()));
	return commandList;
}

void TikzCommandInserter::loadCommands()
{
	if (!m_tikzSections.commands.isEmpty())
		return; // don't load the commands again when opening a second window

	QXmlStreamReader xml;
	m_tikzSections = getCommands(&xml, &m_tikzCommandsList);
}

/***************************************************************************/

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

QMenu *TikzCommandInserter::getMenu(const TikzCommandList &commandList, QWidget *parent)
{
	QMenu *menu = new QMenu(commandList.title, parent);
	const int numOfCommands = commandList.commands.size();
	QAction *action;
	int whichSection = 0;
	QString longestDescription;

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
				menu->addMenu(getMenu(commandList.children.at(whichSection), parent));
				++whichSection;
			}
		}
		else // add command
		{
			action = new QAction(name, menu);
			action->setData(commandList.commands.at(i).number); // link to the corresponding item in m_tikzCommandsList
			action->setStatusTip(commandList.commands.at(i).description);
			if (commandList.commands.at(i).description.size() > longestDescription.size())
				longestDescription = commandList.commands.at(i).description;
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
		action = new QAction(menu);
		action->setSeparator(true);
		menu->addAction(action);

		action = new QAction(menu);
		QFont actionFont = action->font();
		actionFont.setPointSize(actionFont.pointSize() - 1);
		action->setFont(actionFont);
		connect(action, SIGNAL(triggered()), this, SLOT(insertTag()));
		menu->addAction(action);

		// make sure that the menu width does not change when the content
		// of the above menu item changes
		menu->setMinimumWidth(menu->width() + QFontMetrics(actionFont).boundingRect(longestDescription).width());
//		menu->setMinimumWidth(menu->width() + QFontMetrics(QFont()).boundingRect(longestDescription).width()); // faster than the above, but less accurate :(
//		action->setText(longestDescription);
//		menu->setMinimumWidth(menu->sizeHint().width()); // very accurate, but so slow :(
//		action->setText("");
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
	return getMenu(m_tikzSections, qobject_cast<QWidget*>(parent()));
}

//@}

/*!
 * \name Dock widget with TikZ commands
 */
//@{

void TikzCommandInserter::addListWidgetItems(QListWidget *listWidget, const QPalette &standardPalette, const TikzCommandList &commandList, bool addChildren)
{
	QFont titleFont = qApp->font();
	titleFont.setBold(true);
//	QColor titleBg(standardPalette.color(QPalette::Normal, QPalette::Highlight));
//	titleBg = titleBg.lighter(120);
	QColor titleBg(standardPalette.color(QPalette::Normal, QPalette::Window));
	titleBg = titleBg.darker(200);
	QColor titleFg(standardPalette.color(QPalette::Normal, QPalette::HighlightedText));
//	QColor separatorBg(standardPalette.color(QPalette::Normal, QPalette::AlternateBase));
//	if (separatorBg == standardPalette.color(QPalette::Normal, QPalette::Base))
//		separatorBg = separatorBg.darker(110);

	for (int i = 0; i < commandList.commands.size(); ++i)
	{
		if (commandList.commands.at(i).type == -1) // if we have an empty command corresponding to a submenu, then don't add the command, the submenus will be added later
			continue;
		QString itemText = commandList.commands.at(i).name;
		if (itemText.isEmpty())
			continue;

		QListWidgetItem *item = new QListWidgetItem(listWidget);
		item->setText(itemText.remove('&'));

//		if (itemText.isEmpty())
//			item->setBackgroundColor(separatorBg);
//		else
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

		addListWidgetItems(listWidget, standardPalette, commandList.children.at(i));
	}
}

void TikzCommandInserter::showItemsInDockWidget()
{
	QListWidget *tikzListWidget = qobject_cast<QListWidget*>(m_commandsStack->widget(0));
	QPalette standardPalette = QApplication::style()->standardPalette(); // this is slow, so we call this only once here and pass this as argument to addListWidgetItems instead of calling this each time in addListWidgetItems
	addListWidgetItems(tikzListWidget, standardPalette, m_tikzSections, false); // don't add children

	for (int i = 0; i < m_tikzSections.children.size(); ++i)
	{
		QListWidget *tikzListWidget = new QListWidget;
		addListWidgetItems(tikzListWidget, standardPalette, m_tikzSections.children.at(i));
		tikzListWidget->setMouseTracking(true);
		connect(tikzListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
		connect(tikzListWidget, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
		connect(tikzListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));
//		connect(tikzListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));

		QString comboItemText = m_tikzSections.children.at(i).title;
		m_commandsCombo->addItem(comboItemText.remove('&'));
		m_commandsStack->addWidget(tikzListWidget);
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
	m_commandsCombo = new ComboBox;
	m_commandsCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_commandsStack = new QStackedWidget;
	connect(m_commandsCombo, SIGNAL(currentIndexChanged(int)), m_commandsStack, SLOT(setCurrentIndex(int)));

	QListWidget *tikzListWidget = new QListWidget;
	tikzListWidget->setMouseTracking(true);
	connect(tikzListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
	connect(tikzListWidget, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(setListStatusTip(QListWidgetItem*)));
	connect(tikzListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));
//	connect(tikzListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(insertTag(QListWidgetItem*)));
	m_commandsCombo->addItem(tr("General"));
	m_commandsStack->addWidget(tikzListWidget);

	QGridLayout *tikzLayout = new QGridLayout;
	tikzLayout->addWidget(commandsComboLabel, 0, 0);
	tikzLayout->addWidget(m_commandsCombo, 0, 1);
	tikzLayout->addWidget(m_commandsStack, 1, 0, 1, 2);
	tikzLayout->setMargin(5);

	TikzCommandWidget *tikzWidget = new TikzCommandWidget;
	tikzWidget->setLayout(tikzLayout);
	tikzDock->setWidget(tikzWidget);
	tikzDock->setFocusProxy(m_commandsCombo);

	return tikzDock;
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
	QStringList highlightTypeNames = getHighlightTypeNames();

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
	QStringList highlightTypeNames;
	highlightTypeNames << "Commands" << "Draw to" << "Options";
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
	QStringList highlightTypeNames = getHighlightTypeNames();

	for (int i = 0; i < m_tikzCommandsList.size(); ++i)
	{
		QString command = m_tikzCommandsList.at(i).command;
		const int type = m_tikzCommandsList.at(i).type;
		int end;
		if (!m_tikzCommandsList.at(i).highlightString.isEmpty())
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

void TikzCommandInserter::setEditor(QPlainTextEdit *textEdit)
{
	m_mainEdit = textEdit;
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
