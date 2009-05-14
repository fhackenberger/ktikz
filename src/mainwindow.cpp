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

#include <QDebug>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QCompleter>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStatusBar>
#include <QStringListModel>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>
#include <QWhatsThis>

#include "aboutdialog.h"
#include "configdialog.h"
#include "loghighlighter.h"
#include "logtextedit.h"
#include "mainwindow.h"
#include "tikzeditorhighlighter.h"
#include "tikzeditorview.h"
#include "tikzpreview.h"

#include <poppler-qt4.h>

MainWindow::MainWindow()
{
	m_aboutDialog = 0;
	m_configDialog = 0;
	m_tikzPdfDoc = 0;

	setWindowIcon(QIcon(":/images/ktikz-22.png"));
	setAttribute(Qt::WA_DeleteOnClose);

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	m_tikzEditorView = new TikzEditorView(this);
	m_commandInserter = new TikzCommandInserter(this);
	m_commandInserter->setEditor(m_tikzEditorView->editor());
	m_tikzHighlighter = new TikzHighlighter(m_commandInserter, m_tikzEditorView->editor()->document());
	m_tikzHighlighter->rehighlight(); // avoid that textEdit emits the signal contentsChanged() when it is still empty
	m_tikzView = new TikzPreview(this);
	m_tikzController = new TikzPngPreviewer(m_tikzEditorView->editor());

	m_logDock = new QDockWidget(this);
	m_logDock->setObjectName("LogDock");
	m_logDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	m_logDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	m_logDock->setWindowTitle(tr("Messages"));
	addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
	m_logTextEdit = new LogTextEdit;
	m_logTextEdit->setWhatsThis("<p>" + tr("The messages produced by "
	    "LaTeX are shown here.  If your TikZ code contains errors, "
	    "then a red border will appear and the errors will be "
	    "highlighted.") + "</p>");
	m_logTextEdit->setReadOnly(true);
	m_logHighlighter = new LogHighlighter(m_logTextEdit->document());
	m_logDock->setWidget(m_logTextEdit);

	m_previewDock = new QDockWidget(this);
	m_previewDock->setObjectName("PreviewDock");
	m_previewDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	m_previewDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	m_previewDock->setWindowTitle(tr("Preview"));
	m_previewDock->setWidget(m_tikzView);
	addDockWidget(Qt::RightDockWidgetArea, m_previewDock);

	setCentralWidget(m_tikzEditorView);

	createActions();
	createToolBars(); // run first in order to be able to add file/editToolBar->toggleViewAction() to the menu
	createMenus();
	createCommandInsertWidget(); // must happen after createMenus and before readSettings
	createStatusBar();

	connect(m_tikzEditorView, SIGNAL(modificationChanged(bool)),
	        this, SLOT(setDocumentModified(bool)));
	connect(m_tikzEditorView, SIGNAL(cursorPositionChanged(int,int)),
	        this, SLOT(showCursorPosition(int,int)));
	connect(m_tikzController, SIGNAL(pixmapUpdated(Poppler::Document*)),
	        m_tikzView, SLOT(pixmapUpdated(Poppler::Document*)));
	connect(m_tikzController, SIGNAL(shortLogUpdated(QString,bool)),
	        m_logTextEdit, SLOT(logUpdated(QString,bool)));

	readSettings(); // must be run after defining tikzController and tikzHighlighter, and after creating the toolbars, and after the connects

	// the following connects must happen after readSettings() because otherwise in that function the following signals would be unnecessarily triggered
	connect(m_tikzEditorView, SIGNAL(contentsChanged()),
	        m_tikzController, SLOT(regeneratePreview()));
	connect(m_tikzEditorView, SIGNAL(templateFileChanged(QString)),
	        m_tikzController, SLOT(setTemplateFileAndRegenerate(QString)));

	setCurrentFile("");
	setDocumentModified(false);
	m_tikzEditorView->editor()->setFocus();

	QDir dir(QDir::tempPath() + "/ktikz");
	if (!dir.exists())
		QDir::temp().mkdir("ktikz");
}

MainWindow::~MainWindow()
{
	writeSettings();

	delete m_tikzController;
	m_logHighlighter->deleteLater();
	m_tikzHighlighter->deleteLater();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave())
	{
		QApplication::restoreOverrideCursor();
		event->accept();
	}
	else
		event->ignore();
}

void MainWindow::newFile()
{
	MainWindow *newMainWindow = new MainWindow;
	newMainWindow->show();
}

void MainWindow::closeFile()
{
	if (maybeSave())
	{
		m_tikzEditorView->editor()->clear();
		setCurrentFile("");
		m_tikzController->abortProcess(); // abort still running processes
		m_tikzView->pixmapUpdated(0); // empty preview
		m_logTextEdit->logUpdated("", false); // clear log window
	}
}

void MainWindow::open()
{
	QString lastDir;
	if (!m_lastDocument.isEmpty())
	{
		QFileInfo lastFileInfo(m_lastDocument);
		lastDir = lastFileInfo.absolutePath();
	}
	QString filter = QString("%1 (*.pgf *.tikz *.tex);;%2 (*.*)").arg(tr("PGF files")).arg(tr("All files"));
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open PGF source file"), lastDir, filter);
	if (!fileName.isEmpty())
		loadFile(fileName);
}

bool MainWindow::save()
{
	if (m_currentFile.isEmpty())
		return saveAs();
	else
		return saveFile(m_currentFile);
}

bool MainWindow::saveAs()
{
	QString lastDir;
	if (!m_lastDocument.isEmpty())
	{
		QFileInfo lastFileInfo(m_lastDocument);
		lastDir = (m_currentFile.isEmpty()) ? lastFileInfo.absolutePath() : lastFileInfo.absoluteFilePath();
	}
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save PGF source file"), lastDir);
	if (fileName.isEmpty())
		return false;

	return saveFile(fileName);
}

bool MainWindow::exportImage()
{
	QAction *action = qobject_cast<QAction*>(sender());

	QPixmap tikzImage = m_tikzView->getPixmap();
	if (tikzImage.isNull()) return false;

	QString lastDir;
	if (!m_lastDocument.isEmpty())
	{
		QFileInfo lastFileInfo(m_lastDocument);
		lastDir = lastFileInfo.absolutePath() + "/" + lastFileInfo.completeBaseName() + "." + action->data().toString();
	}
	QString filter = QString("%1 (*." + action->data().toString() + ");;%2 (*.*)").arg(action->text().remove('&')).arg(tr("All files"));
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export image"), lastDir, filter);
	if (!fileName.isEmpty())
	{
		QString type = action->data().toString();
		if (type == "png")
		{
			if (!fileName.endsWith(".png"))
				fileName = fileName + ".png";
			return tikzImage.save(fileName, "PNG");
		}
		else
			return m_tikzController->exportImage(fileName, type);
	}
	return false;
}

/***************************************************************************/

void MainWindow::showTikzDocumentation()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QSettings settings;
	QString tikzDocFile = settings.value("TikzDocumentation").toString();
	if (tikzDocFile.isEmpty())
		QMessageBox::warning(this, tr("TikZ editor"),
		                     tr("Cannot find TikZ documentation."));
	else
		QDesktopServices::openUrl(QUrl("file://" + tikzDocFile));

	QApplication::restoreOverrideCursor();
}

void MainWindow::about()
{
	if (!m_aboutDialog)
		m_aboutDialog = new AboutDialog(this);
	m_aboutDialog->exec();
}

/***************************************************************************/

void MainWindow::setDocumentModified(bool isModified)
{
	setWindowModified(isModified);
	m_saveAction->setEnabled(isModified);
	if (m_currentFile.isEmpty() && !isModified)
		m_saveAsAction->setEnabled(false);
	else
		m_saveAsAction->setEnabled(true);
}

void MainWindow::logUpdated()
{
	m_logTextEdit->logUpdated(m_tikzController->getLogText(), m_tikzController->hasRunFailed());
}

void MainWindow::setProcessRunning(bool isRunning)
{
	m_procStopAction->setEnabled(isRunning);
	if (isRunning)
		QApplication::setOverrideCursor(Qt::BusyCursor);
	else
		QApplication::restoreOverrideCursor();
	m_tikzView->setProcessRunning(isRunning);
}

/***************************************************************************/

void MainWindow::createActions()
{
	/* Open */

	QString newWhatsThis = tr("Create a new document");
	m_newAction = new QAction(QIcon(":/images/document-new.png"), tr("&New"), this);
	m_newAction->setShortcut(QKeySequence::New);
	m_newAction->setStatusTip(newWhatsThis);
	m_newAction->setWhatsThis("<p>" + newWhatsThis + "</p>");
	connect(m_newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	QString openWhatsThis = tr("Open an existing file");
	m_openAction = new QAction(QIcon(":/images/document-open.png"), tr("&Open..."), this);
	m_openAction->setShortcut(QKeySequence::Open);
	m_openAction->setStatusTip(openWhatsThis);
	m_openAction->setWhatsThis("<p>" + openWhatsThis + "</p>");
	connect(m_openAction, SIGNAL(triggered()), this, SLOT(open()));

	QString saveWhatsThis = tr("Save the current document to disk");
	m_saveAction = new QAction(QIcon(":/images/document-save.png"), tr("&Save"), this);
	m_saveAction->setShortcut(QKeySequence::Save);
	m_saveAction->setStatusTip(saveWhatsThis);
	m_saveAction->setWhatsThis("<p>" + saveWhatsThis + "</p>");
	connect(m_saveAction, SIGNAL(triggered()), this, SLOT(save()));

	QString saveAsWhatsThis = tr("Save the document under a new name");
	m_saveAsAction = new QAction(QIcon(":/images/document-save-as.png"), tr("Save &As..."), this);
	m_saveAsAction->setStatusTip(saveAsWhatsThis);
	m_saveAsAction->setWhatsThis("<p>" + saveAsWhatsThis + "</p>");
	connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

	QString closeWhatsThis = tr("Close the current document");
	m_closeAction = new QAction(QIcon(":/images/window-close.png"), tr("&Close"), this);
	m_closeAction->setShortcut(QKeySequence::Close);
	m_closeAction->setStatusTip(closeWhatsThis);
	m_closeAction->setToolTip(tr("Close File"));
	m_closeAction->setWhatsThis("<p>" + closeWhatsThis + "</p>");
	connect(m_closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

	QString exitWhatsThis = tr("Exit the application");
	m_exitAction = new QAction(QIcon(":/images/application-exit.png"), tr("&Quit"), this);
	m_exitAction->setShortcut(tr("Ctrl+Q", "File|Quit"));
	m_exitAction->setStatusTip(exitWhatsThis);
	m_exitAction->setWhatsThis("<p>" + exitWhatsThis + "</p>");
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

	m_exportEpsAction = new QAction(QIcon(":/images/image-x-eps.png"), tr("Encapsulated PostScript (EPS)"), this);
	m_exportEpsAction->setData("eps");
	m_exportEpsAction->setStatusTip(tr("Export to PostScript"));
	connect(m_exportEpsAction, SIGNAL(triggered()), this, SLOT(exportImage()));

	m_exportPdfAction = new QAction(QIcon(":/images/application-pdf.png"), tr("Portable Document Format (PDF)"), this);
	m_exportPdfAction->setData("pdf");
	m_exportPdfAction->setStatusTip(tr("Export to PDF"));
	connect(m_exportPdfAction, SIGNAL(triggered()), this, SLOT(exportImage()));

	m_exportPngAction = new QAction(QIcon(":/images/image-png.png"), tr("Portable Network Graphics (PNG)"), this);
	m_exportPngAction->setData("png");
	m_exportPngAction->setStatusTip(tr("Export to PNG"));
	connect(m_exportPngAction, SIGNAL(triggered()), this, SLOT(exportImage()));

	/* View */

	m_procStopAction = new QAction(QIcon(":/images/process-stop.png"), tr("&Stop Process"), this);
	m_procStopAction->setShortcut(tr("Escape", "View|Stop process"));
	m_procStopAction->setStatusTip(tr("Abort current process"));
	m_procStopAction->setWhatsThis("<p>" + tr("Abort the execution of the currently running process.") + "</p>");
	m_procStopAction->setEnabled(false);
	connect(m_procStopAction, SIGNAL(triggered()), m_tikzController, SLOT(abortProcess()));

	m_viewLogAction = new QAction(QIcon(":/images/run-build.png"), tr("View &Log"), this);
	m_viewLogAction->setStatusTip(tr("View log messages produced by the last executed process"));
	m_viewLogAction->setWhatsThis("<p>" + tr("Show the log messages produced by the last executed process in the Messages box.") + "</p>");
	connect(m_viewLogAction, SIGNAL(triggered()), this, SLOT(logUpdated()));

	m_shellEscapeAction = new QAction(QIcon(":/images/application-x-executable.png"), tr("S&hell Escape"), this);
	m_shellEscapeAction->setStatusTip(tr("Enable the \\write18{shell-command} feature"));
	m_shellEscapeAction->setWhatsThis("<p>" + tr("Enable LaTeX to run shell commands, this is needed when you want to plot functions using gnuplot within TikZ.")
	    + "</p><p><strong>" + tr("Warning:") + "</strong> " + tr("Enabling this may cause malicious software to be run on your computer! Check the LaTeX code to see which commands are executed.") + "</p>");
	connect(m_shellEscapeAction, SIGNAL(triggered()), this, SLOT(toggleShellEscaping()));

	/* Configure */

	m_configureAction = new QAction(QIcon(":/images/configure.png"), tr("&Configure..."), this);
	m_configureAction->setStatusTip(tr("Configure the settings of this application"));
	connect(m_configureAction, SIGNAL(triggered()), this, SLOT(configure()));

	/* Help */

	m_showTikzDocAction = new QAction(QIcon(":/images/help-contents.png"), tr("TikZ &Manual"), this);
	m_showTikzDocAction->setStatusTip(tr("Show the manual of TikZ and PGF"));
	connect(m_showTikzDocAction, SIGNAL(triggered()), this, SLOT(showTikzDocumentation()));

	m_aboutAction = new QAction(QIcon(":/images/ktikz-22.png"), tr("&About"), this);
	m_aboutAction->setStatusTip(tr("Show the application's About box"));
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	m_aboutQtAction = new QAction(QIcon(":/images/qt-logo-22.png"), tr("About &Qt"), this);
	m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	connect(m_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	m_whatsThisAction = QWhatsThis::createAction(this);
	m_whatsThisAction->setIcon(QIcon(":/images/help-contextual.png"));
	m_whatsThisAction->setStatusTip(tr("Show simple description of any widget"));

	connect(m_tikzController, SIGNAL(processRunning(bool)),
	        this, SLOT(setProcessRunning(bool)));
}

void MainWindow::createMenus()
{
	m_recentMenu = new QMenu(tr("Open &Recent"), this);
	m_recentMenu->setIcon(QIcon(":/images/document-open-recent.png"));
	m_recentMenu->menuAction()->setStatusTip(tr("Open a recently opened file"));

	QMenu *exportMenu = new QMenu(tr("E&xport"), this);
	exportMenu->setIcon(QIcon(":/images/document-export.png"));
	exportMenu->menuAction()->setStatusTip(tr("Export image to various formats"));
	exportMenu->addAction(m_exportEpsAction);
	exportMenu->addAction(m_exportPdfAction);
	exportMenu->addAction(m_exportPngAction);

	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(m_newAction);
	fileMenu->addAction(m_openAction);
	fileMenu->addMenu(m_recentMenu);
	fileMenu->addSeparator();
	fileMenu->addAction(m_saveAction);
	fileMenu->addAction(m_saveAsAction);
	fileMenu->addMenu(exportMenu);
	fileMenu->addSeparator();
	fileMenu->addAction(m_closeAction);
	fileMenu->addSeparator();
	fileMenu->addAction(m_exitAction);

	menuBar()->addMenu(m_tikzEditorView->createMenu());

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addActions(m_tikzView->getActions());
	viewMenu->addSeparator();
	viewMenu->addAction(m_procStopAction);
	viewMenu->addAction(m_viewLogAction);

	m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
	QMenu *toolBarMenu = new QMenu(tr("&Toolbars"), this);
	toolBarMenu->setIcon(QIcon(":/images/configure-toolbars.png"));
	toolBarMenu->menuAction()->setStatusTip(tr("Show or hide toolbars"));
	toolBarMenu->addAction(m_fileToolBar->toggleViewAction());
	toolBarMenu->addAction(m_editToolBar->toggleViewAction());
	toolBarMenu->addAction(m_viewToolBar->toggleViewAction());
	toolBarMenu->addAction(m_runToolBar->toggleViewAction());
	m_settingsMenu->addMenu(toolBarMenu);
	m_sideBarMenu = new QMenu(tr("&Sidebars"), this);
	m_sideBarMenu->setIcon(QIcon(":/images/configure-toolbars.png"));
	m_sideBarMenu->menuAction()->setStatusTip(tr("Show or hide sidebars"));
	m_sideBarMenu->addAction(m_previewDock->toggleViewAction());
	m_sideBarMenu->addAction(m_logDock->toggleViewAction());
	m_settingsMenu->addMenu(m_sideBarMenu);
	m_settingsMenu->addSeparator();
	m_settingsMenu->addAction(m_configureAction);
	connect(m_fileToolBar->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setToolBarStatusTip(bool)));
	connect(m_editToolBar->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setToolBarStatusTip(bool)));
	connect(m_viewToolBar->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setToolBarStatusTip(bool)));
	connect(m_runToolBar->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setToolBarStatusTip(bool)));
	connect(m_previewDock->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setDockWidgetStatusTip(bool)));
	connect(m_logDock->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setDockWidgetStatusTip(bool)));

	menuBar()->addSeparator();

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(m_showTikzDocAction);
	helpMenu->addAction(m_whatsThisAction);
	helpMenu->addSeparator();
	helpMenu->addAction(m_aboutAction);
	helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::createToolBars()
{
	m_fileToolBar = addToolBar(tr("File"));
	m_fileToolBar->setObjectName("FileToolBar");
	m_fileToolBar->addAction(m_newAction);
	m_fileToolBar->addAction(m_openAction);
	m_fileToolBar->addAction(m_saveAction);
	m_fileToolBar->addAction(m_closeAction);

	m_editToolBar = m_tikzEditorView->createToolBar();
	addToolBar(m_editToolBar);

	m_viewToolBar = m_tikzView->getViewToolBar();
	addToolBar(m_viewToolBar);

	m_runToolBar = addToolBar(tr("Run"));
	m_runToolBar->setObjectName("RunToolBar");
	m_runToolBar->addAction(m_procStopAction);
	m_runToolBar->addAction(m_viewLogAction);

	m_shellEscapeButton = new QToolButton(this);
	m_shellEscapeButton->setDefaultAction(m_shellEscapeAction);
	m_shellEscapeButton->setCheckable(true);
	m_runToolBar->addWidget(m_shellEscapeButton);

	setToolBarStyle();
}

void MainWindow::setToolBarStyle()
{
	QSettings settings;
	settings.beginGroup("MainWindow");

	int toolBarStyleNumber = settings.value("ToolBarStyle", 0).toInt();
	Qt::ToolButtonStyle toolBarStyle = Qt::ToolButtonIconOnly;
	switch (toolBarStyleNumber)
	{
		case 0: toolBarStyle = Qt::ToolButtonIconOnly; break;
		case 1: toolBarStyle = Qt::ToolButtonTextOnly; break;
		case 2: toolBarStyle = Qt::ToolButtonTextBesideIcon; break;
		case 3: toolBarStyle = Qt::ToolButtonTextUnderIcon; break;
	}

	m_fileToolBar->setToolButtonStyle(toolBarStyle);
	m_editToolBar->setToolButtonStyle(toolBarStyle);
	m_viewToolBar->setToolButtonStyle(toolBarStyle);
	m_runToolBar->setToolButtonStyle(toolBarStyle);
	m_shellEscapeButton->setToolButtonStyle(toolBarStyle);
}

void MainWindow::createCommandInsertWidget()
{
	QSettings settings;
	bool commandsInDock = settings.value("CommandsInDock", false).toBool();
	if (commandsInDock)
	{
		m_commandsDock = m_commandInserter->getDockWidget(this);
		addDockWidget(Qt::LeftDockWidgetArea, m_commandsDock);
		connect(m_commandInserter, SIGNAL(showStatusMessage(QString,int)), statusBar(), SLOT(showMessage(QString,int)));

		m_sideBarMenu->insertAction(m_sideBarMenu->actions().at(1), m_commandsDock->toggleViewAction());
		connect(m_commandsDock->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(setDockWidgetStatusTip(bool)));
	}
	else
		menuBar()->insertMenu(m_settingsMenu->menuAction(), m_commandInserter->getMenu());
}

void MainWindow::createStatusBar()
{
	QStatusBar *status = statusBar();

	m_positionLabel = new QLabel(status);
	m_positionLabel->setMinimumWidth(150);
	status->addPermanentWidget(m_positionLabel, 0);

	status->showMessage(tr("Ready"));
}

void MainWindow::setDockWidgetStatusTip(bool enabled)
{
	QAction *action = qobject_cast<QAction*>(sender());
	QString dockName = action->text();
	dockName.remove('&');
	if (enabled)
		action->setStatusTip(tr("Hide sidebar \"%1\"").arg(dockName));
	else
		action->setStatusTip(tr("Show sidebar \"%1\"").arg(dockName));
}

void MainWindow::setToolBarStatusTip(bool enabled)
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (enabled)
		action->setStatusTip(tr("Hide toolbar \"%1\"").arg(action->text()));
	else
		action->setStatusTip(tr("Show toolbar \"%1\"").arg(action->text()));
}

/***************************************************************************/

void MainWindow::configure()
{
	if (!m_configDialog)
	{
		m_configDialog = new ConfigDialog(this);
		m_configDialog->setTranslatedHighlightTypeNames(m_tikzHighlighter->getTranslatedHighlightTypeNames());
		m_configDialog->setHighlightTypeNames(m_tikzHighlighter->getHighlightTypeNames());
		m_configDialog->setDefaultHighlightFormats(m_tikzHighlighter->getDefaultHighlightFormats());
		connect(m_configDialog, SIGNAL(settingsChanged()), this, SLOT(applySettings()));
	}
	m_configDialog->readSettings();
	m_configDialog->exec();
}

void MainWindow::toggleShellEscaping()
{
	m_useShellEscaping = !m_shellEscapeButton->isChecked();
	m_shellEscapeButton->setChecked(m_useShellEscaping);

	QSettings settings;
	settings.setValue("UseShellEscaping", m_useShellEscaping);

	m_tikzController->setShellEscaping(m_useShellEscaping);
	m_tikzController->generatePreview();
}

void MainWindow::applySettings()
{
	QSettings settings;

	m_numOfRecentFiles = settings.value("RecentFilesNumber", 5).toInt();
	QString latexCommand = settings.value("LatexCommand", "pdflatex").toString();
	QString pdftopsCommand = settings.value("PdftopsCommand", "pdftops").toString();
	m_tikzController->setLatexCommand(latexCommand);
	m_tikzController->setPdftopsCommand(pdftopsCommand);

	m_useShellEscaping = settings.value("UseShellEscaping", false).toBool();
	m_shellEscapeButton->setChecked(m_useShellEscaping);
	m_tikzController->setShellEscaping(m_useShellEscaping);

	QString replaceText = settings.value("TemplateReplaceText", "<>").toString();
	m_tikzEditorView->setReplaceText(replaceText);
	m_tikzController->setReplaceText(replaceText); // first set replaceText before setting templateFile
	QString templateFile = settings.value("TemplateFile").toString();
	m_tikzEditorView->setTemplateFile(templateFile);
	m_tikzController->setTemplateFile(templateFile);

	m_tikzEditorView->applySettings();

	settings.beginGroup("Highlighting");
	bool customHighlighting = settings.value("Customize", true).toBool();
	QMap<QString, QTextCharFormat> formatList = m_tikzHighlighter->getDefaultHighlightFormats();
	if (customHighlighting)
	{
		int numOfRules = settings.value("Number", 0).toInt();
		for (int i = 0; i < numOfRules; ++i)
		{
			QString name = settings.value("Item" + QString::number(i) + "/Name").toString();
			QString colorName = settings.value("Item" + QString::number(i) + "/Color").toString();
			QString fontName = settings.value("Item" + QString::number(i) + "/Font").toString();
			QFont font;
			font.fromString(fontName);
			QTextCharFormat format;
			format.setForeground(QBrush(QColor(colorName)));
			format.setFont(font);
			formatList[name] = format;
		}
	}
	settings.endGroup();

	settings.beginGroup("Editor");
	m_useCompletion = settings.value("UseCompletion", true).toBool();
	if (m_useCompletion)
	{
		m_completer = new QCompleter(this);
		updateCompleter();
		m_tikzEditorView->setCompleter(m_completer);
	}
	settings.endGroup();

	m_tikzHighlighter->setTextCharFormats(formatList);
	m_tikzHighlighter->rehighlight();

	createRecentFilesList();
	setToolBarStyle();
}

void MainWindow::readSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
//	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
//	move(pos);
	QSize size = settings.value("size", QSize(800, 600)).toSize();
	resize(size);
	restoreState(settings.value("MainWindowState").toByteArray());
	settings.endGroup();

	m_recentFilesList = settings.value("RecentFiles").toStringList();
	m_lastDocument = settings.value("LastDocument").toString();

	applySettings();
}

void MainWindow::writeSettings()
{
	QSettings settings;

	settings.setValue("TemplateFile", m_tikzEditorView->templateFile());

	settings.beginGroup("MainWindow");
//	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("MainWindowState", saveState());
/*
	int toolBarStyleNumber = 0;
	switch (m_fileToolBar->toolButtonStyle())
	{
		case Qt::ToolButtonIconOnly: toolBarStyleNumber = 0; break;
		case Qt::ToolButtonTextOnly: toolBarStyleNumber = 1; break;
		case Qt::ToolButtonTextBesideIcon: toolBarStyleNumber = 2; break;
		case Qt::ToolButtonTextUnderIcon: toolBarStyleNumber = 3; break;
	}
	settings.setValue("ToolBarStyle", toolBarStyleNumber);
*/
	settings.endGroup();

	if (m_recentFilesList.size() > 0)
		settings.setValue("RecentFiles", m_recentFilesList);
	settings.setValue("LastDocument", m_lastDocument);
}

/***************************************************************************/

bool MainWindow::maybeSave()
{
	if (m_tikzEditorView->editor()->document()->isModified())
	{
		int ret = QMessageBox::warning(this, tr("TikZ editor"),
		                               tr("The document has been modified.\n"
		                                  "Do you want to save your changes?"),
		                               QMessageBox::Yes | QMessageBox::Default,
		                               QMessageBox::No,
		                               QMessageBox::Cancel | QMessageBox::Escape);
		if (ret == QMessageBox::Yes)
			return save();
		else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void MainWindow::loadFile(const QString &fileName)
{
	if (!m_tikzEditorView->editor()->document()->isEmpty())
	{
		MainWindow *newMainWindow = new MainWindow;
		newMainWindow->loadFile(fileName);
		newMainWindow->show();
		return;
	}

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("TikZ editor"),
		                     tr("Cannot read file %1:\n%2.")
		                     .arg(fileName)
		                     .arg(file.errorString()));
		return;
	}

	disconnect(m_tikzEditorView, SIGNAL(contentsChanged()),
	        m_tikzController, SLOT(regeneratePreview()));
	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	m_tikzEditorView->editor()->setPlainText(in.readAll());
	QApplication::restoreOverrideCursor();
	m_tikzController->generatePreview();
	connect(m_tikzEditorView, SIGNAL(contentsChanged()),
	        m_tikzController, SLOT(regeneratePreview()));

	m_lastDocument = fileName;
	setCurrentFile(fileName);
	addToRecentFilesList(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("TikZ editor"),
		                     tr("Cannot write file %1:\n%2.")
		                     .arg(fileName)
		                     .arg(file.errorString()));
		return false;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << m_tikzEditorView->editor()->toPlainText();
	QApplication::restoreOverrideCursor();

	m_lastDocument = fileName;
	setCurrentFile(fileName);
	addToRecentFilesList(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
		loadFile(action->data().toString());
}

void MainWindow::addToRecentFilesList(const QString &fileName)
{
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

void MainWindow::createRecentFilesList()
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

void MainWindow::updateRecentFilesList()
{
	if (m_recentFilesList.count() > 0)
		m_recentMenu->setEnabled(true);
	else
		m_recentMenu->setEnabled(false);

	for (int i = 0; i < m_recentFilesList.count(); ++i)
	{
		m_recentFileActions[i]->setText(m_recentFilesList.at(i));
		m_recentFileActions[i]->setData(m_recentFilesList.at(i));
		m_recentFileActions[i]->setVisible(true);
	}
	for (int i = m_recentFilesList.count(); i < m_numOfRecentFiles; ++i)
		m_recentFileActions[i]->setVisible(false);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	m_currentFile = fileName;
	m_tikzEditorView->editor()->document()->setModified(false);
	setDocumentModified(false);

	QString shownName;
	if (m_currentFile.isEmpty())
		shownName = "untitled.txt";
	else
		shownName = strippedName(m_currentFile);

	setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("TikZ editor")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

/***************************************************************************/

void MainWindow::showCursorPosition(int row, int col)
{
	m_positionLabel->setText(tr("Line:") + " " + QString::number(row) + "\t" + tr("Col:") + " " + QString::number(col));
}

/***************************************************************************/

void MainWindow::updateCompleter()
{
	QStringList words = m_commandInserter->getCommandWords();
	QStringListModel *model = new QStringListModel(words, m_completer);
	m_completer->setModel(model);
	m_completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
	m_completer->setCaseSensitivity(Qt::CaseSensitive);
	m_completer->setWrapAround(false);
}
