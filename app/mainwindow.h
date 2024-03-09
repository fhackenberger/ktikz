/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010, 2012, 2014                      *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
 *   Copyright (C) 2016 by G. Prudhomme                                    *
 *     <gprud@users.noreply.github.com>                                    *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef KTIKZ_USE_KDE
#include <KXmlGuiWindow>
#else
#include <QtCore/QtGlobal>
#include <QtWidgets/QMainWindow>

class AboutDialog;
class AssistantController;
#endif

#include <QtCore/QDateTime>
#include <QtCore/QPointer>
#include "../common/mainwidget.h"

class QAction;
class QCloseEvent;
class QDockWidget;
class QLabel;
class QMenu;
class QToolButton;

class Action;
class ConfigDialog;
class LogTextEdit;
class RecentFilesAction;
class TikzCommandInserter;
class TikzKTextEditorView;
class TikzEditorView;
class TikzEditorViewAbstract;
class TikzHighlighter;
class TikzPreviewController;
class UserCommandInserter;

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
	/*!
	 * This enum is used to determine when the file is checked for external changes.
	 */
	enum FileCheckMoment
	{
		FocusIn, /*!<During focusing in. */
		Saving, /*!<During saving. */
		Closing /*!<During closing. */
	};

	MainWindow();
	virtual ~MainWindow();

	virtual QWidget *widget();
	bool isDocumentModified() const;
	QString tikzCode() const;
	QUrl url() const;
	void setLineNumber(int lineNumber);
	int lineNumber() const;

	static QList<MainWindow*> mainWindowList()
	{
		return s_mainWindowList;
	}

	virtual void configureStreamEncoding(QTextStream &textStream);
	virtual void configureStreamDecoding(QTextStream &textStream);

public Q_SLOTS:
	void loadUrl(const QUrl &url);
	void changedUrl(const QUrl &url);
	bool save();

protected:
#ifdef KTIKZ_USE_KDE
	bool queryClose();
	void readProperties(const KConfigGroup &group);
	void saveProperties(KConfigGroup &group);
#endif
	void closeEvent(QCloseEvent *event);

private Q_SLOTS:
	void init();
	void checkForFileChanges(const FileCheckMoment &moment = FocusIn);
	void saveLastInternalModifiedDateTime();
	void setDockWidgetStatusTip(bool enabled);
	void setToolBarStatusTip(bool enabled);
	void newFile();
	bool closeFile();
	void open();
	bool saveAs();
	void reload();
	void showTikzDocumentation();
#ifndef KTIKZ_USE_KDE
	void about();
	void showDocumentation();
#endif
	void configure();
	void applySettings();
	void setDocumentModified(bool isModified);
	void updateLog();
#ifdef KTIKZ_USE_KDE
	void toggleWhatsThisMode();
#endif
	void showCursorPosition(int row, int col);
	void showMouseCoordinates(qreal x, qreal y, int precisionX = 5, int precisionY = 5);
	void updateCompleter();
	/// Change the codec for the current document
	/// @param isUserRequest set to true if the user requested the changement (in this case, the application should warn the user -- not implemented yet.).
  void setCurrentEncoding(QTextCodec* codec /*, bool isUserRequest = false */ );

private:
	void createActions();
#ifdef KTIKZ_USE_KDE
	void addActionCloneToCollection(const QString &actionName, QAction *action);
#else
	void createMenus();
	void createToolBars();
	void setToolBarStyle();
#endif
	void createCommandInsertWidget();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	bool saveUrl(const QUrl &url);
	void setCurrentUrl(const QUrl &url);
	QString strippedName(const QUrl &url) const;
	void showPdfPage();

	static QList<MainWindow*> s_mainWindowList;

	bool m_useKTextEditor;
	TikzEditorViewAbstract *m_tikzEditorView;
#ifdef KTIKZ_USE_KTEXTEDITOR
	TikzKTextEditorView *m_tikzKTextEditor;
#endif
	TikzEditorView *m_tikzQtEditorView;
	TikzHighlighter *m_tikzHighlighter;

	bool m_useCompletion;

	TikzPreviewController *m_tikzPreviewController;
	bool m_buildAutomatically;

	QDockWidget *m_previewDock;

	QDockWidget *m_logDock;
	LogTextEdit *m_logTextEdit;

	QDockWidget *m_commandsDock;
	TikzCommandInserter *m_commandInserter;
	UserCommandInserter *m_userCommandInserter;

	QLabel *m_positionLabel;
	QLabel *m_mouseCoordinatesLabel;

	QMenu *m_settingsMenu;
	QMenu *m_sideBarMenu;
	QToolBar *m_fileToolBar;
	QToolBar *m_editToolBar;
	QToolBar *m_viewToolBar;
	QToolBar *m_runToolBar;
//#ifndef KTIKZ_USE_KTEXTEDITOR
	QAction *m_saveAction;
	QAction *m_saveAsAction;
	Action *m_reloadAction;
//#endif
	QAction *m_newAction;
	QAction *m_openAction;
	RecentFilesAction *m_openRecentAction;
	QAction *m_exportAction;
	QAction *m_exportEpsAction;
	QAction *m_exportPdfAction;
	QAction *m_exportPngAction;
	QAction *m_closeAction;
	QAction *m_exitAction;
	QAction *m_procStopAction;
	Action *m_buildAction;
	Action *m_insertAction;
	QAction *m_viewLogAction;
	QAction *m_shellEscapeAction;
	QAction *m_configureAction;
	QAction *m_showTikzDocAction;
	QAction *m_whatsThisAction;
#ifndef KTIKZ_USE_KDE
	QAction *m_helpAction;
	QAction *m_aboutAction;
	QAction *m_aboutQtAction;
#endif
	QToolButton *m_shellEscapeButton;
	bool m_useShellEscaping;

#ifndef KTIKZ_USE_KDE
	QPointer<AboutDialog> m_aboutDialog;
	AssistantController *m_assistantController;
#endif
	QPointer<ConfigDialog> m_configDialog;

	QUrl m_currentUrl;
	QTextCodec* m_currentEncoding;
	/// If not null, override the encoder (rather than @ref m_currentEncoding)
	QTextCodec* m_overrideEncoder;
	/// If not null, override the decoder
	QTextCodec* m_overrideDecoder;
	/// True if a BOM must be added to the PGF-file
	bool m_encoderBom;
	/// Return the current encoder (m_currentEncoding or another if encoder is overriden).
	/*virtual*/ QTextCodec* getEncoder() const;

	QUrl m_lastUrl;
	QDateTime m_lastInternalModifiedDateTime;
	bool m_isModifiedExternally;
};

#endif
