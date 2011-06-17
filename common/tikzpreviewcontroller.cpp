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

#include "tikzpreviewcontroller.h"

#ifndef KTIKZ_USE_KDE
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#endif

#include <QtGui/QMenu>
#include <QtCore/QPointer>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtCore/QSettings>
#include <QtCore/QTimer>

#include "templatewidget.h"
#include "tikzpreview.h"
#include "tikzpreviewgenerator.h"
#include "tikztemporaryfilecontroller.h"
#include "mainwidget.h"
#include "utils/action.h"
#include "utils/file.h"
#include "utils/filedialog.h"
#include "utils/icon.h"
#include "utils/messagebox.h"
#include "utils/standardaction.h"
#include "utils/toggleaction.h"

static const int s_minUpdateInterval = 1000; // 1 sec

TikzPreviewController::TikzPreviewController(MainWidget *mainWidget)
{
	m_mainWidget = mainWidget;
	m_parentWidget = m_mainWidget->widget();

	m_templateWidget = new TemplateWidget(m_parentWidget);

	m_tikzPreview = new TikzPreview(m_parentWidget);
	m_tikzPreviewGenerator = new TikzPreviewGenerator(this);

	createActions();

	qRegisterMetaType<QList<qreal> >("QList<qreal>");
	connect(m_tikzPreviewGenerator, SIGNAL(pixmapUpdated(Poppler::Document*,QList<qreal>)),
	        m_tikzPreview, SLOT(pixmapUpdated(Poppler::Document*,QList<qreal>)));
	connect(m_tikzPreviewGenerator, SIGNAL(showErrorMessage(QString)),
	        m_tikzPreview, SLOT(showErrorMessage(QString)));
	connect(m_tikzPreviewGenerator, SIGNAL(setExportActionsEnabled(bool)),
	        this, SLOT(setExportActionsEnabled(bool)));
	connect(m_tikzPreviewGenerator, SIGNAL(shortLogUpdated(QString,bool)),
	        this, SIGNAL(logUpdated(QString,bool)));
	connect(m_templateWidget, SIGNAL(fileNameChanged(QString)),
	        this, SLOT(setTemplateFileAndRegenerate(QString)));
	connect(m_tikzPreview, SIGNAL(showMouseCoordinates(qreal,qreal,int,int)),
	        this, SIGNAL(showMouseCoordinates(qreal,qreal,int,int)));

	m_regenerateTimer = new QTimer(this);
	m_regenerateTimer->setSingleShot(true);
	connect(m_regenerateTimer, SIGNAL(timeout()),
	        this, SLOT(regeneratePreview()));

	m_temporaryFileController = new TikzTemporaryFileController(this);
	m_tikzPreviewGenerator->setTikzFileBaseName(m_temporaryFileController->baseName());
#ifdef KTIKZ_USE_KDE
	File::setMainWidget(m_parentWidget);
	File::setTempDir(m_temporaryFileController->dirName()); // this must happen before any object of type File is constructed
#endif
}

TikzPreviewController::~TikzPreviewController()
{
	delete m_tikzPreviewGenerator;
}

/***************************************************************************/

const QString TikzPreviewController::tempDir() const
{
	return m_temporaryFileController->dirName();
}

/***************************************************************************/

TemplateWidget *TikzPreviewController::templateWidget() const
{
	return m_templateWidget;
}

TikzPreview *TikzPreviewController::tikzPreview() const
{
	return m_tikzPreview;
}

/***************************************************************************/

void TikzPreviewController::createActions()
{
	// File
	m_exportAction = new Action(Icon("document-export"), tr("E&xport"), m_parentWidget, "file_export_as");
	m_exportAction->setStatusTip(tr("Export image to various formats"));
	m_exportAction->setWhatsThis(tr("<p>Export image to various formats.</p>"));
	QMenu *exportMenu = new QMenu(m_parentWidget);
	m_exportAction->setMenu(exportMenu);

	Action *exportEpsAction = new Action(Icon("image-x-eps"), tr("&Encapsulated PostScript (EPS)"), m_parentWidget, "file_export_eps");
	exportEpsAction->setData("image/x-eps");
	exportEpsAction->setStatusTip(tr("Export to EPS"));
	exportEpsAction->setWhatsThis(tr("<p>Export to EPS.</p>"));
	connect(exportEpsAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	exportMenu->addAction(exportEpsAction);

	Action *exportPdfAction = new Action(Icon("application-pdf"), tr("&Portable Document Format (PDF)"), m_parentWidget, "file_export_pdf");
	exportPdfAction->setData("application/pdf");
	exportPdfAction->setStatusTip(tr("Export to PDF"));
	exportPdfAction->setWhatsThis(tr("<p>Export to PDF.</p>"));
	connect(exportPdfAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	exportMenu->addAction(exportPdfAction);

	QStringList mimeTypes;
	QStringList mimeTypeNames;
	mimeTypes << "png" << "jpeg" << "tiff" << "bmp";
	mimeTypeNames << tr("Portable Network &Graphics") << tr("&Joint Photographic Experts Group Format") << tr("&Tagged Image File Format") << tr("&Windows Bitmap");
	for (int i = 0; i < mimeTypes.size(); ++i)
	{
		Action *exportImageAction = new Action(Icon("image-" + mimeTypes.at(i)), mimeTypeNames.at(i) + " (" + mimeTypes.at(i).toUpper() + ")", m_parentWidget, "file_export_" + mimeTypes.at(i));
		exportImageAction->setData("image/" + mimeTypes.at(i));
		exportImageAction->setStatusTip(tr("Export to %1").arg(mimeTypes.at(i).toUpper()));
		exportImageAction->setWhatsThis(tr("<p>Export to %1.</p>").arg(mimeTypes.at(i).toUpper()));
		connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportImage()));
		exportMenu->addAction(exportImageAction);
	}

#ifndef KTIKZ_KPART // don't have two "Print" actions in the kpart
	m_printAction = StandardAction::print(this, SLOT(printImage()), this);
	m_printAction->setStatusTip(tr("Print image"));
	m_printAction->setWhatsThis(tr("<p>Print preview image.</p>"));
#endif

	setExportActionsEnabled(false);

	// View
	m_procStopAction = new Action(Icon("process-stop"), tr("&Stop Process"), m_parentWidget, "stop_process");
	m_procStopAction->setShortcut(tr("Escape", "View|Stop Process"));
	m_procStopAction->setStatusTip(tr("Abort current process"));
	m_procStopAction->setWhatsThis(tr("<p>Abort the execution of the currently running process.</p>"));
	m_procStopAction->setEnabled(false);
	connect(m_procStopAction, SIGNAL(triggered()), this, SLOT(abortProcess()));

	m_shellEscapeAction = new ToggleAction(Icon("application-x-executable"), tr("S&hell Escape"), m_parentWidget, "shell_escape");
	m_shellEscapeAction->setStatusTip(tr("Enable the \\write18{shell-command} feature"));
	m_shellEscapeAction->setWhatsThis(tr("<p>Enable LaTeX to run shell commands, this is needed when you want to plot functions using gnuplot within TikZ."
	                                     "</p><p><strong>Warning:</strong> Enabling this may cause malicious software to be run on your computer! Check the LaTeX code to see which commands are executed.</p>"));
	connect(m_shellEscapeAction, SIGNAL(toggled(bool)), this, SLOT(toggleShellEscaping(bool)));

	connect(m_tikzPreviewGenerator, SIGNAL(processRunning(bool)),
	        this, SLOT(setProcessRunning(bool)));
}

#ifndef KTIKZ_USE_KDE
QAction *TikzPreviewController::exportAction()
{
	return m_exportAction;
}

QAction *TikzPreviewController::printAction()
{
	return m_printAction;
}

QMenu *TikzPreviewController::menu()
{
	QMenu *viewMenu = new QMenu(tr("&View"), m_parentWidget);
	viewMenu->addActions(m_tikzPreview->actions());
	viewMenu->addSeparator();
	viewMenu->addAction(m_procStopAction);
	viewMenu->addAction(m_shellEscapeAction);
	return viewMenu;
}

QList<QToolBar*> TikzPreviewController::toolBars()
{
	QToolBar *toolBar = new QToolBar(tr("Run"), m_parentWidget);
	toolBar->setObjectName("RunToolBar");
	toolBar->addAction(m_procStopAction);

	m_shellEscapeButton = new QToolButton(m_parentWidget);
	m_shellEscapeButton->setDefaultAction(m_shellEscapeAction);
	m_shellEscapeButton->setCheckable(true);
	toolBar->addWidget(m_shellEscapeButton);

	m_toolBars << m_tikzPreview->toolBar() << toolBar;

	return m_toolBars;
}

void TikzPreviewController::setToolBarStyle(const Qt::ToolButtonStyle &style)
{
	for (int i = 0; i < m_toolBars.size(); ++i)
		m_toolBars.at(i)->setToolButtonStyle(style);
	m_shellEscapeButton->setToolButtonStyle(style);
}
#endif

/***************************************************************************/

Url TikzPreviewController::getExportUrl(const Url &url, const QString &mimeType) const
{
	QString currentFile;
	const QString extension = (mimeType == "image/x-eps") ? "eps"
	                          : ((mimeType == "application/pdf") ? "pdf" : mimeType.mid(6));
	if (!url.isEmpty())
	{
		QFileInfo currentFileInfo(url.path());
		currentFile = currentFileInfo.absolutePath();
		if (!currentFile.endsWith('/'))
			currentFile += '/';
		currentFile += currentFileInfo.completeBaseName()
		               + (m_tikzPreview->numberOfPages() > 1 && mimeType != "application/pdf" ? "_" + QString::number(m_tikzPreview->currentPage() + 1) : "")
		               + '.' + extension;
	}
	return FileDialog::getSaveUrl(m_parentWidget, tr("Export image"), Url(currentFile), mimeType);
}

void TikzPreviewController::exportImage()
{
	QAction *action = qobject_cast<QAction*>(sender());
	const QString mimeType = action->data().toString();

	const QPixmap tikzImage = m_tikzPreview->pixmap();
	if (tikzImage.isNull())
		return;

	const Url exportUrl = getExportUrl(m_mainWidget->url(), mimeType);
	if (!exportUrl.isValid())
		return;

	QString extension;
	if (mimeType == "application/pdf")
	{
		extension = ".pdf";
	}
	else if (mimeType == "image/x-eps")
	{
		if (!m_tikzPreviewGenerator->generateEpsFile(m_tikzPreview->currentPage()))
			return;
		extension = ".eps";
	}
	else
	{
		extension = '.' + mimeType.mid(6);
		tikzImage.save(m_temporaryFileController->baseName() + extension);
	}

	if (!File::copy(Url(m_temporaryFileController->baseName() + extension), exportUrl))
		MessageBox::error(m_parentWidget,
		                  tr("The image could not be exported to the file \"%1\".").arg(exportUrl.path()),
		                  QCoreApplication::applicationName());
}

/***************************************************************************/

void TikzPreviewController::printImage()
{
	QPrinter printer;

	QPointer<QPrintDialog> printDialog = new QPrintDialog(&printer, m_parentWidget);
	printDialog->setWindowTitle(tr("Print image"));
	if (printDialog->exec() != QDialog::Accepted)
	{
		delete printDialog;
		return;
	}
	delete printDialog;
	// There does not seem to exist a cross-platform way of printing PDF files directly, so we use the following:
	QPainter painter;
	painter.begin(&printer);
	painter.drawPixmap(0, 0, m_tikzPreview->pixmap());
	painter.end();
}

/***************************************************************************/

bool TikzPreviewController::setTemplateFile(const QString &path)
{
	File templateFile(path, File::ReadOnly);
	if (templateFile.file()->exists()) // use local copy of template file if template file is remote
		m_tikzPreviewGenerator->setTemplateFile(templateFile.file()->fileName());
	else
		m_tikzPreviewGenerator->setTemplateFile(QString());
	return true;
}

void TikzPreviewController::setTemplateFileAndRegenerate(const QString &path)
{
	if (setTemplateFile(path))
		generatePreview(true);
}

void TikzPreviewController::setReplaceTextAndRegenerate(const QString &replace)
{
	m_tikzPreviewGenerator->setReplaceText(replace);
	generatePreview(true);
}

/***************************************************************************/

QString TikzPreviewController::tikzCode() const
{
	return m_mainWidget->tikzCode();
}

QString TikzPreviewController::getLogText()
{
	return m_tikzPreviewGenerator->getLogText();
}

void TikzPreviewController::generatePreview()
{
	generatePreview(true);
}

void TikzPreviewController::generatePreview(bool templateChanged)
{
	if (templateChanged) // old aux files may contain commands available in the old template, but not anymore in the new template
		m_temporaryFileController->cleanUp();

	// the directory in which the pgf file is located is added to TEXINPUTS before running latex
	const QString currentFileName = m_mainWidget->url().path();
	if (!currentFileName.isEmpty())
		m_tikzPreviewGenerator->addToLatexSearchPath(QFileInfo(currentFileName).absolutePath());

	m_tikzPreviewGenerator->abortProcess(); // abort still running process before starting a new one (without this, if a process hangs, all subsequently started processes are executed one after the other when the user aborts the hanging process)
	m_tikzPreviewGenerator->generatePreview(templateChanged);
}

void TikzPreviewController::regeneratePreview()
{
	generatePreview(false);
}

void TikzPreviewController::regeneratePreviewAfterDelay()
{
	// Each start cancels the previous one, this means that timeout() is only
	// fired when there have been no changes in the text editor for the last
	// s_minUpdateInterval msecs. This ensures that the preview is not
	// regenerated on every character that is added/changed/removed.
	m_regenerateTimer->start(s_minUpdateInterval);
}

void TikzPreviewController::emptyPreview()
{
	setExportActionsEnabled(false);
	m_tikzPreviewGenerator->abortProcess(); // abort still running processes
	m_tikzPreview->emptyPreview();
}

void TikzPreviewController::abortProcess()
{
	// We cannot connect the triggered signal of m_procStopAction directly
	// to m_tikzPreviewGenerator->abortProcess() because then the latter would
	// be run in the same thread as the process which must be aborted, so the
	// abortion would be executed after the process finishes.  So we must abort
	// the process in the main thread by calling m_tikzPreviewGenerator->abortProcess()
	// as a regular (non-slot) function.
	m_tikzPreviewGenerator->abortProcess();
}

/***************************************************************************/

void TikzPreviewController::applySettings()
{
	QSettings settings(ORGNAME, APPNAME);
	m_tikzPreviewGenerator->setLatexCommand(settings.value("LatexCommand", "pdflatex").toString());
	m_tikzPreviewGenerator->setPdftopsCommand(settings.value("PdftopsCommand", "pdftops").toString());
	const bool useShellEscaping = settings.value("UseShellEscaping", false).toBool();

	disconnect(m_shellEscapeAction, SIGNAL(toggled(bool)), this, SLOT(toggleShellEscaping(bool)));
	m_shellEscapeAction->setChecked(useShellEscaping);
	m_tikzPreviewGenerator->setShellEscaping(useShellEscaping);
	connect(m_shellEscapeAction, SIGNAL(toggled(bool)), this, SLOT(toggleShellEscaping(bool)));

	setTemplateFile(settings.value("TemplateFile").toString());
	const QString replaceText = settings.value("TemplateReplaceText", "<>").toString();
	m_tikzPreviewGenerator->setReplaceText(replaceText);
	m_templateWidget->setReplaceText(replaceText);
	m_templateWidget->setEditor(settings.value("TemplateEditor", KTIKZ_TEMPLATE_EDITOR_DEFAULT).toString());

	settings.beginGroup("Preview");
	m_tikzPreview->setShowCoordinates(settings.value("ShowCoordinates", true).toBool());
	m_tikzPreview->setCoordinatePrecision(settings.value("ShowCoordinatesPrecision", -1).toInt());
	settings.endGroup();
}

void TikzPreviewController::setExportActionsEnabled(bool enabled)
{
	m_exportAction->setEnabled(enabled);
#ifndef KTIKZ_KPART
	m_printAction->setEnabled(enabled);
#endif
}

void TikzPreviewController::setProcessRunning(bool isRunning)
{
	m_procStopAction->setEnabled(isRunning);
	if (isRunning)
		QApplication::setOverrideCursor(Qt::BusyCursor);
	else
		QApplication::restoreOverrideCursor();
	m_tikzPreview->setProcessRunning(isRunning);
}

void TikzPreviewController::toggleShellEscaping(bool useShellEscaping)
{
#ifndef KTIKZ_USE_KDE
	m_shellEscapeButton->setChecked(useShellEscaping);
#endif

	QSettings settings(ORGNAME, APPNAME);
	settings.setValue("UseShellEscaping", useShellEscaping);

	m_tikzPreviewGenerator->setShellEscaping(useShellEscaping);
	generatePreview(false);
}
