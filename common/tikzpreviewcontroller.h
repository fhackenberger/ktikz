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

#include <QtCore/QObject>
#include "utils/url.h"

#ifndef KTIKZ_USE_KDE
class QAction;
class QMenu;
class QToolBar;
class QToolButton;
#endif

class QTimer;
class MainWidget;
class TemplateWidget;
class TikzPreview;
class TikzPreviewGenerator;
class TikzTemporaryFileController;
class Action;
class ToggleAction;

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
	QAction *printAction();
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
	void regeneratePreviewAfterDelay();

private slots:
	void setTemplateFileAndRegenerate(const QString &path);
	void setReplaceTextAndRegenerate(const QString &replace);
	void regeneratePreview();
	void abortProcess();
	void exportImage();
	void printImage();
	void setExportActionsEnabled(bool enabled);
	void setProcessRunning(bool isRunning);
	void toggleShellEscaping(bool useShellEscaping);

signals:
	void logUpdated(const QString &logText, bool runFailed);
	void showMouseCoordinates(qreal x, qreal y);

private:
	void createActions();
	void generatePreview(bool templateChanged);
	bool setTemplateFile(const QString &path);
	Url getExportUrl(const Url &url, const QString &mimeType) const;

	MainWidget *m_mainWidget;
	QWidget *m_parentWidget;

	TemplateWidget *m_templateWidget;
	TikzPreview *m_tikzPreview;
	TikzPreviewGenerator *m_tikzPreviewGenerator;

	QTimer *m_regenerateTimer;

#ifndef KTIKZ_USE_KDE
	QToolButton *m_shellEscapeButton;
	QList<QToolBar*> m_toolBars;
#endif
	Action *m_exportAction;
	Action *m_printAction;
	Action *m_procStopAction;
	ToggleAction *m_shellEscapeAction;

	TikzTemporaryFileController *m_temporaryFileController;
};

#endif
