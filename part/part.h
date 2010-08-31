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

#ifndef KTIKZ_PART_H
#define KTIKZ_PART_H

#include "../common/mainwidget.h"
#include <KParts/ReadOnlyPart>

class KAboutData;
class KAction;
class KDirWatch;

class PartConfigDialog;
class TikzPreviewController;

/** Provides a kpart for viewing TikZ graphics
 * @author Florian Hackenberger, Glad Deschrijver
 */
class Part : public KParts::ReadOnlyPart, public MainWidget
{
	Q_OBJECT

public:
	Part(QWidget *parentWidget, QObject *parent, const QVariantList &args);
	virtual ~Part();

	static KAboutData *createAboutData();
	virtual QWidget *widget();
	QString tikzCode() const;
	Url url() const;

protected:
	virtual bool openFile();
	bool closeUrl();

private slots:
	void saveAs();
//	void showJobError(KJob *job);
//	void slotReload();

	void slotFileDirty(const QString &path);
	void slotDoFileDirty();

	void applySettings();
	void configure();

private:
	void createActions();

	TikzPreviewController *m_tikzPreviewController;
	PartConfigDialog *m_configDialog;

	KAction *m_saveAsAction;
//	KAction *m_reloadAction;

	QString m_tikzCode;

	// document watcher (and reloader) variables
	KDirWatch *m_watcher;
	QTimer *m_dirtyHandler;
	bool m_fileWasRemoved;
};

#endif
