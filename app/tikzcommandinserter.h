/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2010 by Glad Deschrijver              *
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

#ifndef TIKZCOMMANDINSERTER_H
#define TIKZCOMMANDINSERTER_H

#include <QObject>
#include <QStringList>

class QDockWidget;
class QDomElement;
class QListWidget;
class QListWidgetItem;
class QMenu;
class QPlainTextEdit;
class QTextCharFormat;
class HighlightingRule;

struct TikzCommand
{
	QString name;
	QString description;
	QString command;
	int dx;
	int dy;
	int type;
	int number;
};

struct TikzCommandList
{
	QString title;
	QList<TikzCommand> commands;
	QList<TikzCommandList> children;
};

class TikzCommandInserter : public QObject
{
	Q_OBJECT

public:
	TikzCommandInserter(QWidget *parent = 0);

	QStringList getCommandWords();
	QMenu *getMenu();
	QDockWidget *getDockWidget(QWidget *parent = 0);
	void setEditor(QPlainTextEdit *textEdit);
	QMap<QString, QTextCharFormat> getDefaultHighlightFormats();
	QStringList getTranslatedHighlightTypeNames();
	QStringList getHighlightTypeNames();
	QVector<HighlightingRule> getHighlightingRules();
	void insertTag(const QString &tag, int dx = 0, int dy = 0);

signals:
	/*!
	 * This signal is emitted whenever a message should be shown in
	 * the statusbar.
	 * \param message the message to be shown in the status bar
	 * \param timeout the duration in milli-seconds (if non-zero) during which the message must be shown
	 */
	void showStatusMessage(const QString &message, int timeout = 0);

private:
	void getCommands();
	TikzCommandList getCommands(const QDomElement &element);
	QMenu *getMenu(const TikzCommandList &commandList);
	void addListWidgetItems(QListWidget *listWidget, const TikzCommandList &commandList, bool addChildren = true);
	TikzCommand newCommand(const QString &name, const QString &command, int dx, int dy, int type = 0);
	TikzCommand newCommand(const QString &name, const QString &description, const QString &command, int dx, int dy, int type = 0);

	QPlainTextEdit *m_mainEdit;
	QWidget *m_parentWidget;
	TikzCommandList m_tikzSections;
	QList<TikzCommand> m_tikzCommandsList;

	QStringList highlightTypeNames;

private slots:
	void setListStatusTip(QListWidgetItem *item);
	void insertTag();
	void insertTag(QListWidgetItem *item);
};

#endif
