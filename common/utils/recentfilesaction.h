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

#ifndef KTIKZ_RECENTFILESACTION_H
#define KTIKZ_RECENTFILESACTION_H

class Icon;
class Url;

#ifdef KTIKZ_USE_KDE
#include <KRecentFilesAction>

class KActionCollection;

class RecentFilesAction : public KRecentFilesAction
{
	Q_OBJECT

public:
	RecentFilesAction(QObject *parent);
	RecentFilesAction(const QString &text, QObject *parent);
	RecentFilesAction(const Icon &icon, const QString &text, QObject *parent);

	void createRecentFilesList() {}
	void loadEntries();
	void saveEntries();
	void addUrl(const Url &url, const QString &name = QString());
	void removeUrl(const Url &url);

signals:
	void urlSelected(const Url &url);

private slots:
	void selectUrl(const KUrl &url);
};
#else
#include "action.h"

class RecentFilesAction : public Action
{
	Q_OBJECT

public:
	RecentFilesAction(QObject *parent);
	RecentFilesAction(const QString &text, QObject *parent);
	RecentFilesAction(const Icon &icon, const QString &text, QObject *parent);
	~RecentFilesAction();

	void createRecentFilesList();
	void loadEntries();
	void saveEntries();
	void addUrl(const QUrl &url, const QString &name = QString());
	void removeUrl(const QUrl &url);

signals:
	void urlSelected(const Url &url);

private slots:
	void openRecentFile();

private:
	void createMenu();
	void updateRecentFilesList();

	QMenu *m_recentMenu;
	QList<QAction*> m_recentFileActions;
	QStringList m_recentFilesList;
	int m_numOfRecentFiles;
};
#endif

#endif
