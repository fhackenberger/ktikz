/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#include "recentfilesaction.h"

#include "action.h"
#include "icon.h"
#include "url.h"

#ifdef KTIKZ_USE_KDE
#include <KActionCollection>
#include <KConfigGroup>

RecentFilesAction::RecentFilesAction(QObject *parent)
    : KRecentFilesAction(parent)
{
	Action::actionCollection()->addAction("file_open_recent", this);
	connect(this, SIGNAL(urlSelected(KUrl)), this, SLOT(selectUrl(KUrl)));
}

RecentFilesAction::RecentFilesAction(const QString &text, QObject *parent)
    : KRecentFilesAction(text, parent)
{
	Action::actionCollection()->addAction("file_open_recent", this);
	connect(this, SIGNAL(urlSelected(KUrl)), this, SLOT(selectUrl(KUrl)));
}

RecentFilesAction::RecentFilesAction(const Icon &icon, const QString &text, QObject *parent)
    : KRecentFilesAction(icon, text, parent)
{
	Action::actionCollection()->addAction("file_open_recent", this);
	connect(this, SIGNAL(urlSelected(KUrl)), this, SLOT(selectUrl(KUrl)));
}

void RecentFilesAction::loadEntries()
{
	KRecentFilesAction::loadEntries(KGlobal::config()->group("Recent Files"));
	setEnabled(true);
}

void RecentFilesAction::saveEntries()
{
	KRecentFilesAction::saveEntries(KGlobal::config()->group("Recent Files"));
	KGlobal::config()->sync();
}

void RecentFilesAction::selectUrl(const KUrl &url)
{
	emit urlSelected(Url(url));
}

void RecentFilesAction::addUrl(const Url &url, const QString &name)
{
	KRecentFilesAction::addUrl(url, name);
}

void RecentFilesAction::removeUrl(const Url &url)
{
	KRecentFilesAction::removeUrl(url);
}
#else
#include <QMenu>
#include <QSettings>

RecentFilesAction::RecentFilesAction(QObject *parent)
    : Action(parent)
{
	createMenu();
}

RecentFilesAction::RecentFilesAction(const QString &text, QObject *parent)
    : Action(text, parent)
{
	createMenu();
}

RecentFilesAction::RecentFilesAction(const Icon &icon, const QString &text, QObject *parent)
    : Action(icon, text, parent)
{
	createMenu();
}

RecentFilesAction::~RecentFilesAction()
{
	delete m_recentMenu;
}

void RecentFilesAction::createMenu()
{
	setObjectName("file_open_recent");
	setText(tr("Open &Recent"));
	setIcon(Icon("document-open-recent"));

	m_recentMenu = new QMenu();
	setMenu(m_recentMenu);
}

void RecentFilesAction::openRecentFile()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
		emit urlSelected(Url("file://" + action->data().toString()));
}

void RecentFilesAction::createRecentFilesList()
{
	m_recentFileActions.clear();
	QAction *action;
	for (int i = 0; i < m_numOfRecentFiles; ++i)
	{
		action = new QAction(this);
		action->setVisible(false);
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
		m_recentFileActions.append(action);
	}

	// when the user has decreased the maximum number of recent files, then we must remove the superfluous entries
	while (m_recentFilesList.size() > m_numOfRecentFiles)
		m_recentFilesList.removeLast();

	updateRecentFilesList();

	m_recentMenu->clear();
	m_recentMenu->addActions(m_recentFileActions);
}

void RecentFilesAction::loadEntries()
{
	QSettings settings;
	m_numOfRecentFiles = settings.value("RecentFilesNumber", 5).toInt();

	m_recentFilesList = settings.value("RecentFiles").toStringList();
	setEnabled(true);
}

void RecentFilesAction::saveEntries()
{
	QSettings settings;
	if (m_recentFilesList.size() > 0)
		settings.setValue("RecentFiles", m_recentFilesList);
}

void RecentFilesAction::updateRecentFilesList()
{
	m_recentMenu->setEnabled(m_recentFilesList.count() > 0);

	for (int i = 0; i < m_recentFilesList.count(); ++i)
	{
		m_recentFileActions[i]->setText(m_recentFilesList.at(i));
		m_recentFileActions[i]->setData(m_recentFilesList.at(i));
		m_recentFileActions[i]->setVisible(true);
	}
	for (int i = m_recentFilesList.count(); i < m_numOfRecentFiles; ++i)
		m_recentFileActions[i]->setVisible(false);
}

void RecentFilesAction::addUrl(const QUrl &url, const QString &name)
{
	Q_UNUSED(name);
	const QString fileName = url.path();

	if (m_recentFilesList.contains(fileName))
		m_recentFilesList.move(m_recentFilesList.indexOf(fileName), 0);
	else
	{
		if (m_recentFilesList.count() >= m_numOfRecentFiles)
			m_recentFilesList.removeLast();
		m_recentFilesList.prepend(fileName);
	}
	updateRecentFilesList();
}

void RecentFilesAction::removeUrl(const QUrl &url)
{
	m_recentFilesList.removeAll(url.path());
	updateRecentFilesList();
}
#endif
