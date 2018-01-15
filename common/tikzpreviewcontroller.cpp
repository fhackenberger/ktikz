/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014                      *
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

#include "tikzpreviewcontroller.h"

#ifndef KTIKZ_USE_KDE
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QToolBar>
#else
#include <QtGui/QToolBar>
#endif
#endif

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#else
#include <QtGui/QDialogButtonBox>
#include <QtGui/QMenu>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QPushButton>
// #include <KConfigGroup> // JC
#endif

#include "templatewidget.h"
#include "tikzpreview.h"
#include "mainwidget.h"
#include "utils/action.h"
#include "utils/file.h"
#include "utils/filedialog.h"
#include "utils/icon.h"
#include "utils/messagebox.h"
#include "utils/printpreviewdialog.h"
#include "utils/standardaction.h"
#include "utils/tempdir.h"
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
	connect(m_tikzPreviewGenerator, SIGNAL(updateLog(QString,bool)),
	        this, SIGNAL(updateLog(QString,bool)));
	connect(m_tikzPreviewGenerator, SIGNAL(appendLog(QString,bool)),
	        this, SIGNAL(appendLog(QString,bool)));
	connect(m_templateWidget, SIGNAL(fileNameChanged(QString)),
	        this, SLOT(setTemplateFileAndRegenerate(QString)));
	connect(m_tikzPreview, SIGNAL(showMouseCoordinates(qreal,qreal,int,int)),
	        this, SIGNAL(showMouseCoordinates(qreal,qreal,int,int)));

	m_regenerateTimer = new QTimer(this);
	m_regenerateTimer->setSingleShot(true);
	connect(m_regenerateTimer, SIGNAL(timeout()),
	        this, SLOT(regeneratePreview()));

	m_tempDir = new TempDir();
	m_tikzPreviewGenerator->setTikzFileBaseName(tempFileBaseName());
#ifdef KTIKZ_USE_KDE
	File::setMainWidget(m_parentWidget);
	File::setTempDir(m_tempDir->path()); // this must happen before any object of type File is constructed
#endif
}

TikzPreviewController::~TikzPreviewController()
{
	delete m_tikzPreviewGenerator;
	delete m_tempDir;
}

/***************************************************************************/

const TextCodecProfile *TikzPreviewController::textCodecProfile() const {
	return (TextCodecProfile*) this->m_mainWidget;
}


/***************************************************************************/

const QString TikzPreviewController::tempDir() const
{
	return m_tempDir->path();
}

const QString TikzPreviewController::tempFileBaseName() const
{
	return m_tempDir->path() + QLatin1String("/temptikzcode");
}

const QString TikzPreviewController::tempDirLocation() const
{
	return m_tempDir->location();
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
	m_exportAction = new Action(Icon(QLatin1String("document-export")), tr("E&xport"), m_parentWidget, QLatin1String("file_export_as"));
	m_exportAction->setStatusTip(tr("Export image to various formats"));
	m_exportAction->setWhatsThis(tr("<p>Export image to various formats.</p>"));
	QMenu *exportMenu = new QMenu(m_parentWidget);
	m_exportAction->setMenu(exportMenu);

	Action *exportEpsAction = new Action(Icon(QLatin1String("image-x-eps")), tr("&Encapsulated PostScript (EPS)"), exportMenu, QLatin1String("file_export_eps"));
	exportEpsAction->setData(QLatin1String("image/x-eps"));
	exportEpsAction->setStatusTip(tr("Export to EPS"));
	exportEpsAction->setWhatsThis(tr("<p>Export to EPS.</p>"));
	connect(exportEpsAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	exportMenu->addAction(exportEpsAction);

	Action *exportPdfAction = new Action(Icon(QLatin1String("application-pdf")), tr("&Portable Document Format (PDF)"), exportMenu, QLatin1String("file_export_pdf"));
	exportPdfAction->setData(QLatin1String("application/pdf"));
	exportPdfAction->setStatusTip(tr("Export to PDF"));
	exportPdfAction->setWhatsThis(tr("<p>Export to PDF.</p>"));
	connect(exportPdfAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	exportMenu->addAction(exportPdfAction);

	QStringList mimeTypes;
	QStringList mimeTypeNames;
	mimeTypes << QLatin1String("png") << QLatin1String("jpeg") << QLatin1String("tiff") << QLatin1String("bmp");
	mimeTypeNames << tr("Portable Network &Graphics") << tr("&Joint Photographic Experts Group Format") << tr("&Tagged Image File Format") << tr("&Windows Bitmap");
	for (int i = 0; i < mimeTypes.size(); ++i)
	{
		Action *exportImageAction = new Action(Icon(QLatin1String("image-") + mimeTypes.at(i)), mimeTypeNames.at(i) + QLatin1String(" (") + mimeTypes.at(i).toUpper() + QLatin1Char(')'), exportMenu, QLatin1String("file_export_") + mimeTypes.at(i));
		exportImageAction->setData(QLatin1String("image/") + mimeTypes.at(i));
		exportImageAction->setStatusTip(tr("Export to %1").arg(mimeTypes.at(i).toUpper()));
		exportImageAction->setWhatsThis(tr("<p>Export to %1.</p>").arg(mimeTypes.at(i).toUpper()));
		connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportImage()));
		exportMenu->addAction(exportImageAction);
	}

#ifndef KTIKZ_KPART // don't have two "Print" actions in the kpart
	m_printPreviewAction = StandardAction::printPreview(this, SLOT(printPreviewImage()), this);
	m_printPreviewAction->setStatusTip(tr("Print preview image"));
	m_printPreviewAction->setWhatsThis(tr("<p>Show print preview of the preview image.</p>"));

	m_printAction = StandardAction::print(this, SLOT(printImage()), this);
	m_printAction->setStatusTip(tr("Print image"));
	m_printAction->setWhatsThis(tr("<p>Print the preview image.</p>"));
#endif

	setExportActionsEnabled(false);

	// View
	m_procStopAction = new Action(Icon(QLatin1String("process-stop")), tr("&Stop Process"), m_parentWidget, QLatin1String("stop_process"));
	m_procStopAction->setShortcut(tr("Escape", "View|Stop Process"));
	m_procStopAction->setStatusTip(tr("Abort current process"));
	m_procStopAction->setWhatsThis(tr("<p>Abort the execution of the currently running process.</p>"));
	m_procStopAction->setEnabled(false);
	connect(m_procStopAction, SIGNAL(triggered()), this, SLOT(abortProcess()));

	m_shellEscapeAction = new ToggleAction(Icon(QLatin1String("application-x-executable")), tr("S&hell Escape"), m_parentWidget, QLatin1String("shell_escape"));
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

QAction *TikzPreviewController::printPreviewAction()
{
	return m_printPreviewAction;
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
	toolBar->setObjectName(QLatin1String("RunToolBar"));
	toolBar->addAction(m_procStopAction);
	toolBar->addAction(m_shellEscapeAction);

	m_toolBars << m_tikzPreview->toolBar() << toolBar;

	return m_toolBars;
}

void TikzPreviewController::setToolBarStyle(const Qt::ToolButtonStyle &style)
{
	for (int i = 0; i < m_toolBars.size(); ++i)
		m_toolBars.at(i)->setToolButtonStyle(style);
}
#endif

/***************************************************************************/

Url TikzPreviewController::getExportUrl(const Url &url, const QString &mimeType) const
{
	QString currentFile;
	const QString extension = mimeType == QLatin1String("image/x-eps") ? QLatin1String("eps")
	                          : (mimeType == QLatin1String("application/pdf") ? QLatin1String("pdf") : mimeType.mid(6));
	if (!url.isEmpty())
	{
		const QFileInfo currentFileInfo(url.path());
		currentFile = currentFileInfo.absolutePath();
		if (!currentFile.endsWith(QLatin1Char('/')))
			currentFile += QLatin1Char('/');
		currentFile += currentFileInfo.completeBaseName()
		               + (m_tikzPreview->numberOfPages() > 1 && mimeType != QLatin1String("application/pdf") ? QLatin1String("_") + QString::number(m_tikzPreview->currentPage() + 1) : QString())
		               + QLatin1Char('.') + extension;
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
	if (mimeType == QLatin1String("application/pdf"))
	{
		extension = QLatin1String(".pdf");
	}
	else if (mimeType == QLatin1String("image/x-eps"))
	{
		if (!m_tikzPreviewGenerator->generateEpsFile(m_tikzPreview->currentPage()))
		{
			MessageBox::error(m_parentWidget, tr("Export failed."), QCoreApplication::applicationName());
			return;
		}
		extension = QLatin1String(".eps");
	}
	else
	{
		extension = QLatin1Char('.') + mimeType.mid(6);
		if (!tikzImage.save(tempFileBaseName() + extension))
		{
			MessageBox::error(m_parentWidget, tr("Export failed."), QCoreApplication::applicationName());
			return;
		}
	}

	if (!File::copy(Url(tempFileBaseName() + extension), exportUrl))
		MessageBox::error(m_parentWidget,
		                  tr("The image could not be exported to the file \"%1\".").arg(exportUrl.path()),
		                  QCoreApplication::applicationName());
}

/***************************************************************************/

void TikzPreviewController::printImage(QPrinter *printer)
{
	// get page range
	int startPage, endPage;
	if (printer->printRange() == QPrinter::PageRange)
	{
		startPage = printer->fromPage() - 1;
		endPage = printer->toPage() - 1;
	}
	else if (printer->printRange() == QPrinter::CurrentPage)
	{
		startPage = m_tikzPreview->currentPage();
		endPage = m_tikzPreview->currentPage();
	}
	else
	{
		startPage = 0;
		endPage = m_tikzPreview->numberOfPages() - 1;
	}

	// print
	QPainter painter;
	painter.begin(printer);
//	painter.drawPixmap(0, 0, m_tikzPreview->pixmap());
	for (int i = startPage; i <= endPage; ++i)
	{
		if (i != startPage)
			printer->newPage();
		const QImage image = m_tikzPreview->renderToImage(printer->physicalDpiX(), printer->physicalDpiY(), i);
		if (!image.isNull())
		{
			const double scaleFactor = qMin(double(painter.window().width()) / image.width(), double(painter.window().height()) / image.height());
			painter.drawImage(QRect(0, 0, image.width() * scaleFactor, image.height() * scaleFactor), image, image.rect());
		}
	}
	painter.end();
}

void TikzPreviewController::printPreviewImage()
{
	QPrinter printer;

	// choose printer
	QPointer<QPrintDialog> printDialog = new QPrintDialog(&printer, m_parentWidget);
	printDialog->setWindowTitle(tr("Print preview of image"));
	printDialog->setOptions(printDialog->options() | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintCurrentPage);
	printDialog->setMinMax(0, m_tikzPreview->numberOfPages());
	QList<QDialogButtonBox*> dialogButtonBoxes = printDialog->findChildren<QDialogButtonBox*>();
	QPushButton *printButton = dialogButtonBoxes.at(0)->button(QDialogButtonBox::Ok);
	printButton->setText(tr("Print &preview"));
	if (printDialog->exec() != QDialog::Accepted)
	{
		delete printDialog;
		return;
	}
	delete printDialog;

	// show print preview
	PrintPreviewDialog preview(&printer);
	connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printImage(QPrinter*)));
	preview.exec();
}

void TikzPreviewController::printImage()
{
	QPrinter printer(QPrinter::PrinterResolution);

	// choose printer
	QPointer<QPrintDialog> printDialog = new QPrintDialog(&printer, m_parentWidget);
	printDialog->setWindowTitle(tr("Print image"));
	printDialog->setOptions(printDialog->options() | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintCurrentPage);
	printDialog->setMinMax(0, m_tikzPreview->numberOfPages());
	if (printDialog->exec() != QDialog::Accepted)
	{
		delete printDialog;
		return;
	}
	delete printDialog;

	printImage(&printer);
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
		generatePreview(TikzPreviewGenerator::ReloadTemplate);
}

void TikzPreviewController::setReplaceTextAndRegenerate(const QString &replace)
{
	m_tikzPreviewGenerator->setReplaceText(replace);
	generatePreview(TikzPreviewGenerator::ReloadTemplate);
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
	generatePreview(TikzPreviewGenerator::ReloadTemplate);
}

void TikzPreviewController::generatePreview(TikzPreviewGenerator::TemplateStatus templateStatus)
{
	if (templateStatus == TikzPreviewGenerator::ReloadTemplate) // old aux files may contain commands available in the old template, but not anymore in the new template
		m_tempDir->cleanUp();

	// the directory in which the pgf file is located is added to TEXINPUTS (and the directory of the old pgf file is removed) before running latex
	const QString currentFileName = m_mainWidget->url().path();
	if (!m_currentFileName.isEmpty() && currentFileName != m_currentFileName)
		m_tikzPreviewGenerator->removeFromLatexSearchPath(QFileInfo(m_currentFileName).absolutePath());
	m_currentFileName = currentFileName;
	if (!currentFileName.isEmpty())
		m_tikzPreviewGenerator->addToLatexSearchPath(QFileInfo(currentFileName).absolutePath());

	m_tikzPreviewGenerator->abortProcess(); // abort still running process before starting a new one (without this, if a process hangs, all subsequently started processes are executed one after the other when the user aborts the hanging process)
	m_tikzPreviewGenerator->generatePreview(templateStatus);
}

void TikzPreviewController::regeneratePreview()
{
	generatePreview(TikzPreviewGenerator::DontReloadTemplate);
}

void TikzPreviewController::regeneratePreviewAfterDelay()
{
	if (tikzCode().isEmpty())
	{
		m_tikzPreview->pixmapUpdated(0); // clean up error messages in preview
		Q_EMIT updateLog(QString(), false); // clean up error messages in log panel
	}
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
	QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
	m_tikzPreviewGenerator->setLatexCommand(settings.value(QLatin1String("LatexCommand"), QLatin1String("pdflatex")).toString());
	m_tikzPreviewGenerator->setPdftopsCommand(settings.value(QLatin1String("PdftopsCommand"), QLatin1String("pdftops")).toString());
	const bool useShellEscaping = settings.value(QLatin1String("UseShellEscaping"), false).toBool();

	disconnect(m_shellEscapeAction, SIGNAL(toggled(bool)), this, SLOT(toggleShellEscaping(bool)));
	m_shellEscapeAction->setChecked(useShellEscaping);
	m_tikzPreviewGenerator->setShellEscaping(useShellEscaping);
	connect(m_shellEscapeAction, SIGNAL(toggled(bool)), this, SLOT(toggleShellEscaping(bool)));

	setTemplateFile(settings.value(QLatin1String("TemplateFile")).toString());
	const QString replaceText = settings.value(QLatin1String("TemplateReplaceText"), QLatin1String("<>")).toString();
	m_tikzPreviewGenerator->setReplaceText(replaceText);
	m_templateWidget->setReplaceText(replaceText);
	m_templateWidget->setEditor(settings.value(QLatin1String("TemplateEditor"), QLatin1String("")).toString());

	settings.beginGroup(QLatin1String("Preview"));
	m_tikzPreview->setShowCoordinates(settings.value(QLatin1String("ShowCoordinates"), true).toBool());
	m_tikzPreview->setCoordinatePrecision(settings.value(QLatin1String("ShowCoordinatesPrecision"), -1).toInt());
	settings.endGroup();
}

void TikzPreviewController::setExportActionsEnabled(bool enabled)
{
	m_exportAction->setEnabled(enabled);
#ifndef KTIKZ_KPART
	m_printPreviewAction->setEnabled(enabled);
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
	QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
	settings.setValue(QLatin1String("UseShellEscaping"), useShellEscaping);

	m_tikzPreviewGenerator->setShellEscaping(useShellEscaping);
	generatePreview(TikzPreviewGenerator::DontReloadTemplate);
}
