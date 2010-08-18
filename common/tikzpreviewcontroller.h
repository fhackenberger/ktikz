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

#ifndef KTIKZ_TIKZPREVIEWCONTROLLER_H
#define KTIKZ_TIKZPREVIEWCONTROLLER_H

#include <QObject>

#ifdef KTIKZ_USE_KDE
#include <KUrl>
class KAction;
class KActionCollection;
class KJob;
class KTempDir;
class KToggleAction;
#else
class QAction;
class QMenu;
class QToolBar;
class QToolButton;
#endif

class TemplateWidget;
class TikzPreview;
class TikzPreviewGenerator;
class MainWidget;

class TikzPreviewController : public QObject
{
	Q_OBJECT

public:
	TikzPreviewController(MainWidget *mainWidget);
	virtual ~TikzPreviewController();

	const QString tempDir() const;
	TemplateWidget *templateWidget() const;
	TikzPreview *tikzPreview() const;
#ifndef KTIKZ_USE_KDE
	QAction *exportAction();
	QMenu *menu();
	QList<QToolBar*> toolBars();
	void setToolBarStyle(const Qt::ToolButtonStyle &style);
#endif
	QString tikzCode() const;
	QString getLogText();
	void emptyPreview();
	void applySettings();

public slots:
	void generatePreview();
	void regeneratePreview();

private slots:
#ifdef KTIKZ_USE_KDE
	void showJobError(KJob *job);
#endif
	void setTemplateFileAndRegenerate(const QString &path);
	void setReplaceTextAndRegenerate(const QString &replace);
	void exportImage();
	void setExportActionsEnabled(bool enabled);
	void setProcessRunning(bool isRunning);
	void toggleShellEscaping(bool useShellEscaping);

signals:
    void logUpdated(const QString &logText, bool runFailed);

private:
	void createTempDir();
	void removeTempDir();

	void createActions();
	bool setTemplateFile(const QString &path);
#ifdef KTIKZ_USE_KDE
	KUrl getExportUrl(const KUrl &url, const QString &mimeType) const;
#else
	QString getExportFileName(const QString &fileName, const QString &mimeType) const;
#endif
	void generatePreview(bool templateChanged);
	bool cleanUp();

	MainWidget *m_mainWidget;
	QWidget *m_parentWidget;

	TemplateWidget *m_templateWidget;
	TikzPreview *m_tikzPreview;
	TikzPreviewGenerator *m_tikzPreviewGenerator;

#ifdef KTIKZ_USE_KDE
	KActionCollection *m_actionCollection;
	KAction *m_exportAction;
	KAction *m_procStopAction;
	KToggleAction *m_shellEscapeAction;

	KTempDir *m_tempDir;
#else
	QAction *m_exportAction;
	QAction *m_procStopAction;
	QAction *m_shellEscapeAction;
	QToolButton *m_shellEscapeButton;
	QList<QToolBar*> m_toolBars;
#endif
	QString m_tempTikzFileBaseName;
};

#endif
