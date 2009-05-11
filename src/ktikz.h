/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *   Copyright (C) 2007-2008 by Glad Deschrijver                           *
 *   florian@hackenberger.at                                               *
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

#ifndef KTIKZ_H
#define KTIKZ_H

#include <QMainWindow>
#include "tikzpngpreviewer.h"
#include "tikzcommandinserter.h"

class QAction;
class QCloseEvent;
class QCompleter;
class QDockWidget;
class QLabel;
class QMenu;
class QSyntaxHighlighter;
class QToolButton;

namespace Poppler
{
	class Document;
}

class AboutDialog;
class ConfigDialog;
class LogTextEdit;
class TikzEditorView;
class TikzHighlighter;
class TikzPreview;

/** Provides a tiny application for simple editing of TikZ graphics
 * @author Florian Hackenberger
 */
class ktikz : public QMainWindow
{
	Q_OBJECT

public:
	ktikz();
	virtual ~ktikz();

	void loadFile(const QString &fileName);

signals:
	void setSearchFromBegin(bool searchFromBegin);

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void setProcessRunning(bool isRunning);
	void setDockWidgetStatusTip(bool enabled);
	void setToolBarStatusTip(bool enabled);
	void newFile();
	void closeFile();
	void open();
	bool save();
	bool saveAs();
	void openRecentFile();
	bool exportImage();
	void showTikzDocumentation();
	void about();
	void configure();
	void toggleShellEscaping();
	void applySettings();
	void setDocumentModified(bool isModified);
	void logUpdated();
	void showCursorPosition(int row, int col);

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void setToolBarStyle();
	void createCommandInsertWidget();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	bool saveFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);
	void addToRecentFilesList(const QString &fileName);
	void createRecentFilesList();
	void updateRecentFilesList();
	void showPdfPage();
	void updateCompleter();

	TikzEditorView *m_tikzEditorView;
	QString m_currentFile;
	TikzHighlighter *m_tikzHighlighter;
	QCompleter *m_completer;
	bool m_useCompletion;

	QDockWidget *m_previewDock;
	Poppler::Document *m_tikzPdfDoc;
	TikzPreview *m_tikzView;
	TikzPngPreviewer *m_tikzController;

	QDockWidget *m_logDock;
	LogTextEdit *m_logTextEdit;
	QSyntaxHighlighter *m_logHighlighter;

	QDockWidget *m_commandsDock;
	TikzCommandInserter *m_commandInserter;

	QLabel *m_positionLabel;

	QMenu *m_recentMenu;
	QMenu *m_settingsMenu;
	QMenu *m_sideBarMenu;
	QToolBar *m_fileToolBar;
	QToolBar *m_editToolBar;
	QToolBar *m_viewToolBar;
	QToolBar *m_runToolBar;
	QAction *m_newAction;
	QAction *m_openAction;
	QAction *m_saveAction;
	QAction *m_saveAsAction;
	QAction *m_exportEpsAction;
	QAction *m_exportPdfAction;
	QAction *m_exportPngAction;
	QAction *m_closeAction;
	QAction *m_exitAction;
	QAction *m_procStopAction;
	QAction *m_viewLogAction;
	QAction *m_shellEscapeAction;
	QAction *m_configureAction;
	QAction *m_showTikzDocAction;
	QAction *m_whatsThisAction;
	QAction *m_aboutAction;
	QAction *m_aboutQtAction;
	QToolButton *m_shellEscapeButton;
	bool m_useShellEscaping;

	AboutDialog *m_aboutDialog;
	ConfigDialog *m_configDialog;

	QList<QAction*> m_recentFileActions;
	QStringList m_recentFilesList;
	int m_numOfRecentFiles;
	QString m_lastDocument;
};

#endif
