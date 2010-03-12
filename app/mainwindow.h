/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *   Copyright (C) 2007-2010 by Glad Deschrijver                           *
 *   florian@hackenberger.at                                               *
 *   glad.deschrijver@gmail.com                                            *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef KTIKZ_USE_KDE
#include <KXmlGuiWindow>
class KUrl;
#else
#include <QMainWindow>
class QUrl;
#endif
#include "../common/mainwidget.h"

class QAction;
class QCloseEvent;
class QCompleter;
class QDockWidget;
class QLabel;
class QMenu;
class QSyntaxHighlighter;
class QToolButton;

class AboutDialog;
class ConfigDialog;
class LogTextEdit;
class RecentFilesAction;
class TikzCommandInserter;
class TikzEditorView;
class TikzHighlighter;
class TikzPreviewController;
class Url;

/** Provides a tiny application for simple editing of TikZ graphics
 * @author Florian Hackenberger
 */
#ifdef KTIKZ_USE_KDE
class MainWindow : public KXmlGuiWindow, public MainWidget
#else
class MainWindow : public QMainWindow, public MainWidget
#endif
{
	Q_OBJECT

public:
	MainWindow();
	virtual ~MainWindow();

	bool isDocumentModified() const;
	QString tikzCode() const;
	Url url() const;

	static QList<MainWindow*> mainWindowList() { return s_mainWindowList; }

public slots:
	void loadUrl(const Url &url);
	bool save();

signals:
	void setSearchFromBegin(bool searchFromBegin);

protected:
#ifdef KTIKZ_USE_KDE
	bool queryClose();
	void readProperties(const KConfigGroup &group);
	void saveProperties(KConfigGroup &group);
#endif
	void closeEvent(QCloseEvent *event);

private slots:
	void setDockWidgetStatusTip(bool enabled);
	void setToolBarStatusTip(bool enabled);
	void newFile();
	void closeFile();
	void open();
	bool saveAs();
	void showTikzDocumentation();
#ifndef KTIKZ_USE_KDE
	void about();
#endif
	void configure();
	void applySettings();
	void setDocumentModified(bool isModified);
	void logUpdated();
#ifdef KTIKZ_USE_KDE
	void toggleWhatsThisMode();
#endif
	void showCursorPosition(int row, int col);

private:
	void createActions();
#ifndef KTIKZ_USE_KDE
	void createMenus();
	void createToolBars();
	void setToolBarStyle();
#endif
	void createCommandInsertWidget();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	bool saveUrl(const Url &url);
	void setCurrentUrl(const Url &url);
	QString strippedName(const Url &url) const;
	void showPdfPage();
	void updateCompleter();

	static QList<MainWindow*> s_mainWindowList;

	TikzEditorView *m_tikzEditorView;
	TikzHighlighter *m_tikzHighlighter;
	QCompleter *m_completer;
	bool m_useCompletion;

	TikzPreviewController *m_tikzPreviewController;

	QDockWidget *m_previewDock;

	QDockWidget *m_logDock;
	LogTextEdit *m_logTextEdit;
	QSyntaxHighlighter *m_logHighlighter;

	QDockWidget *m_commandsDock;
	TikzCommandInserter *m_commandInserter;

	QLabel *m_positionLabel;

	QMenu *m_settingsMenu;
	QMenu *m_sideBarMenu;
	QToolBar *m_fileToolBar;
	QToolBar *m_editToolBar;
	QToolBar *m_viewToolBar;
	QToolBar *m_runToolBar;
	QAction *m_newAction;
	QAction *m_openAction;
	RecentFilesAction *m_openRecentAction;
	QAction *m_saveAction;
	QAction *m_saveAsAction;
	QAction *m_exportAction;
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
#ifndef KTIKZ_USE_KDE
	QAction *m_aboutAction;
	QAction *m_aboutQtAction;
#endif
	QToolButton *m_shellEscapeButton;
	bool m_useShellEscaping;

	AboutDialog *m_aboutDialog;
	ConfigDialog *m_configDialog;

	Url m_currentUrl;
	Url m_lastUrl;
};

#endif
