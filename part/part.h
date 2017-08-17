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

#ifndef KTIKZ_PART_H
#define KTIKZ_PART_H

#include "../common/mainwidget.h"
#include <KParts/Part>
#include <KParts/ReadOnlyPart>

class KAboutData;
class QAction;
class KDirWatch;
class QTranslator;

class TikzPreviewController;

namespace KtikZ
{

class PartConfigDialog;

/** Provides a kpart for viewing TikZ graphics
 * @author Florian Hackenberger, Glad Deschrijver
 */
class Part : public KParts::ReadOnlyPart, public MainWidget
{
	Q_OBJECT

public:
	explicit Part(QWidget *parentWidget, QObject *parent, const QVariantList &args);
	virtual ~Part() Q_DECL_OVERRIDE;

	static KAboutData *createAboutData();
	virtual QWidget *widget();
	QString tikzCode() const;
	Url url() const;

protected:
	/** Reimplemented from KParts::PartBase. */
	bool openFile() Q_DECL_OVERRIDE;
	bool closeUrl();

private slots:
	void showAboutDialog();

	void saveAs();
	void showJobError(KJob *job);

	void slotFileDirty(const QString &path);
	void slotDoFileDirty();

	void applySettings();
	void configure();

private:
	void createActions();
	bool findTranslator(QTranslator *translator, const QString &transName, const QString &transDir);
	QTranslator *createTranslator(const QString &transName);

	TikzPreviewController *m_tikzPreviewController;
	PartConfigDialog *m_configDialog;

	QAction *m_saveAsAction;

	QString m_tikzCode;

	// document watcher (and reloader) variables
	KDirWatch *m_watcher;
	QTimer *m_dirtyHandler;
	bool m_fileWasRemoved;
};

} // namespace KtikZ


#endif
