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

#include "tikzpreviewcontroller.h"

#ifdef KTIKZ_USE_KDE
#include <KDebug>
#include <KFileDialog>
#include <KMessageBox>
#include <KTempDir>
#include <KIO/Job>
#include <KIO/JobUiDelegate>
#include <KIO/NetAccess>
#else
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QToolBar>
#include <QToolButton>
#endif

#include <QMenu>
#include <QPointer>
#include <QSettings>

#include "templatewidget.h"
#include "tikzpreview.h"
#include "tikzpreviewgenerator.h"
#include "mainwidget.h"
#include "utils/action.h"
#include "utils/icon.h"
#include "utils/toggleaction.h"

TikzPreviewController::TikzPreviewController(MainWidget *mainWidget)
{
	m_mainWidget = mainWidget;
	m_parentWidget = m_mainWidget->widget();

	m_templateWidget = new TemplateWidget(m_parentWidget);

	m_tikzPreview = new TikzPreview(m_parentWidget);
	m_tikzPreviewGenerator = new TikzPreviewGenerator(this);

	createActions();

	connect(m_tikzPreviewGenerator, SIGNAL(pixmapUpdated(Poppler::Document*)),
	        m_tikzPreview, SLOT(pixmapUpdated(Poppler::Document*)));
	connect(m_tikzPreviewGenerator, SIGNAL(showErrorMessage(QString)),
	        m_tikzPreview, SLOT(showErrorMessage(QString)));
	connect(m_tikzPreviewGenerator, SIGNAL(setExportActionsEnabled(bool)),
	        this, SLOT(setExportActionsEnabled(bool)));
	connect(m_tikzPreviewGenerator, SIGNAL(shortLogUpdated(QString,bool)),
	        this, SIGNAL(logUpdated(QString,bool)));
	connect(m_templateWidget, SIGNAL(fileNameChanged(QString)),
	        this, SLOT(setTemplateFileAndRegenerate(QString)));
//	connect(m_templateWidget, SIGNAL(replaceTextChanged(QString)),
//	        this, SLOT(setReplaceTextAndRegenerate(QString)));

	createTempDir();

//	applySettings(); // must be done after creation of m_tempDir
}

TikzPreviewController::~TikzPreviewController()
{
	delete m_tikzPreviewGenerator;
	removeTempDir();
}

/***************************************************************************/

void TikzPreviewController::createTempDir()
{
#ifdef KTIKZ_USE_KDE
	m_tempDir = new KTempDir();
	m_tempDir->setAutoRemove(true);
	m_tempTikzFileBaseName = m_tempDir->name() + "temptikzcode";
#else
	QDir tempDir(QDir::tempPath() + "/ktikz");
	if (!tempDir.exists())
		QDir::temp().mkdir("ktikz");
	QTemporaryFile *tempFile = new QTemporaryFile(QDir::tempPath() + "/ktikz/temptikzcodeXXXXXX.tex");
	if (tempFile->open())
	{
		const QFileInfo tempFileInfo = QFileInfo(*tempFile);
		m_tempTikzFileBaseName = tempFileInfo.absolutePath() + '/' + tempFileInfo.completeBaseName();
	}
	else
		qCritical() << "Error: could not create temporary file in" << QDir::toNativeSeparators(QDir::tempPath() + "/ktikz/");
	delete tempFile;
//	m_tempTikzFileBaseName = QDir::tempPath() + "/ktikz/temptikzcode";
#endif
	m_tikzPreviewGenerator->setTikzFileBaseName(m_tempTikzFileBaseName);
}

void TikzPreviewController::removeTempDir()
{
#ifdef KTIKZ_USE_KDE
	delete m_tempDir;
#else
	// removing all temporary files
	qDebug("removing tempfiles");
	if (!m_tempTikzFileBaseName.isEmpty() && !cleanUp())
		qCritical("Error: removing tempfiles failed");

	// remove temp dir if empty
	QDir tempDir(QDir::tempPath() + "/ktikz");
	if (tempDir.exists())
		QDir::temp().rmdir("ktikz");
#endif
}

const QString TikzPreviewController::tempDir() const
{
#ifdef KTIKZ_USE_KDE
	return m_tempDir->name();
#else
	return QDir::tempPath() + "/ktikz";
#endif
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

	Action *exportPngAction = new Action(Icon("image-png"), tr("Portable Network &Graphics (PNG)"), m_parentWidget, "file_export_png");
	exportPngAction->setData("image/png");
	exportPngAction->setStatusTip(tr("Export to PNG"));
	exportPngAction->setWhatsThis(tr("<p>Export to PNG.</p>"));
	connect(exportPngAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	exportMenu->addAction(exportPngAction);

	setExportActionsEnabled(false);

	// View
	m_procStopAction = new Action(Icon("process-stop"), tr("&Stop Process"), m_parentWidget, "stop_process");
	m_procStopAction->setShortcut(tr("Escape", "View|Stop Process"));
	m_procStopAction->setStatusTip(tr("Abort current process"));
	m_procStopAction->setWhatsThis(tr("<p>Abort the execution of the currently running process.</p>"));
	m_procStopAction->setEnabled(false);
	connect(m_procStopAction, SIGNAL(triggered()), m_tikzPreviewGenerator, SLOT(abortProcess()));

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

#ifdef KTIKZ_USE_KDE
void TikzPreviewController::showJobError(KJob *job)
{
	if (job->error() != 0)
	{
		KIO::JobUiDelegate *ui = static_cast<KIO::Job*>(job)->ui();
		if (!ui)
		{
			kError() << "Saving failed; job->ui() is null.";
			return;
		}
		ui->setWindow(m_parentWidget);
		ui->showErrorMessage();
	}
}

KUrl TikzPreviewController::getExportUrl(const KUrl &url, const QString &mimeType) const
{
	KMimeType::Ptr mimeTypePtr = KMimeType::mimeType(mimeType);
	const QString exportUrlExtension = KMimeType::extractKnownExtension(url.path());

	const KUrl exportUrl = KUrl(url.url().left(url.url().length()
	    - (exportUrlExtension.isEmpty() ? 0 : exportUrlExtension.length() + 1)) // the extension is empty when the text/x-pgf mimetype is not correctly installed or when the file does not have a correct extension
	    + mimeTypePtr->patterns().at(0).mid(1)); // first extension in the list of possible extensions (without *)

	return KFileDialog::getSaveUrl(exportUrl,
	    mimeTypePtr->patterns().join(" ") + '|'
//	    + mimeTypePtr->comment() + "\n*|" + i18nc("@item:inlistbox filter", "All files"),
	    + mimeTypePtr->comment() + "\n*|" + tr("All files"),
	    m_parentWidget,
//	    i18nc("@title:window", "Export Image"),
	    tr("Export Image"),
	    KFileDialog::ConfirmOverwrite);
}
#else
QString TikzPreviewController::getExportFileName(const QString &fileName, const QString &mimeType) const
{
	QString currentFile;
	const QString extension = (mimeType == QLatin1String("image/x-eps")) ? "eps"
	    : ((mimeType == QLatin1String("application/pdf")) ? "pdf" : "png");
	const QString mimeTypeName = (mimeType == QLatin1String("image/x-eps")) ? tr("EPS image")
	    : ((mimeType == QLatin1String("application/pdf")) ? tr("PDF document") : tr("PNG image"));
	if (!fileName.isEmpty())
	{
		QFileInfo currentFileInfo(fileName);
		currentFile = currentFileInfo.absolutePath() + '/' + currentFileInfo.completeBaseName() + '.' + extension;
	}
	const QString filter = QString("%1 (*.%2);;%3 (*.*)")
	    .arg(mimeTypeName)
	    .arg(extension)
	    .arg(tr("All files"));
	return QFileDialog::getSaveFileName(m_parentWidget, tr("Export image"), currentFile, filter);
}
#endif

void TikzPreviewController::exportImage()
{
	QAction *action = qobject_cast<QAction*>(sender());
	const QString mimeType = action->data().toString();

	const QPixmap tikzImage = m_tikzPreview->pixmap();
	if (tikzImage.isNull())
		return;

#ifdef KTIKZ_USE_KDE
	const KUrl exportUrl = getExportUrl(m_mainWidget->url(), mimeType);
	if (!exportUrl.isValid())
		return;
#else
	const QString exportFileName = getExportFileName(m_mainWidget->url().path(), mimeType);
	if (exportFileName.isEmpty())
		return;

	QFileInfo exportFileInfo(exportFileName);
	if (exportFileInfo.exists())
	{
		QPointer<QMessageBox> warningBox = new QMessageBox(m_parentWidget);
		warningBox->setWindowTitle(QCoreApplication::applicationName());
		warningBox->setText(tr("A file named \"%1\" already exists.  "
		    "Are you sure you want to overwrite it?").arg(exportFileInfo.fileName()));
		warningBox->setIcon(QMessageBox::Warning);
		QPushButton *overwriteButton = warningBox->addButton(tr("&Overwrite", "Do you want to overwrite an existing file - warning box"), QMessageBox::AcceptRole);
		QPushButton *cancelButton = warningBox->addButton(tr("&Cancel", "Do you want to overwrite an existing file - warning box"), QMessageBox::RejectRole);
		Q_UNUSED(overwriteButton)
		warningBox->exec();
		if (!warningBox || warningBox->clickedButton() == cancelButton)
		{
			delete warningBox;
			return;
		}
		else if (!QFile::remove(exportFileName))
		{
			QMessageBox::critical(m_parentWidget, QCoreApplication::applicationName(),
			    tr("The file \"%1\" could not be overwritten.").arg(exportFileName));
			delete warningBox;
			return;
		}
		delete warningBox;
	}
#endif

	QString extension;
	if (mimeType == QLatin1String("application/pdf"))
	{
		extension = ".pdf";
	}
	else if (mimeType == QLatin1String("image/x-eps"))
	{
		if (!m_tikzPreviewGenerator->generateEpsFile())
			return;
		extension = ".eps";
	}
	else if (mimeType == QLatin1String("image/png"))
	{
		extension = ".png";
		tikzImage.save(m_tempTikzFileBaseName + extension);
	}
#ifdef KTIKZ_USE_KDE
	KIO::Job *job = KIO::file_copy(KUrl::fromPath(m_tempTikzFileBaseName + extension), exportUrl, -1, KIO::Overwrite | KIO::HideProgressInfo);
	connect(job, SIGNAL(result(KJob*)), this, SLOT(showJobError(KJob*)));
#else
	if (!QFile::copy(m_tempTikzFileBaseName + extension, exportFileName))
		QMessageBox::critical(m_parentWidget, QCoreApplication::applicationName(),
		    tr("The image could not be exported to the file \"%1\".").arg(exportFileName));
#endif
}

/***************************************************************************/

bool TikzPreviewController::setTemplateFile(const QString &path)
{
#ifdef KTIKZ_USE_KDE
	const KUrl url(path);
	const KUrl localUrl = KUrl::fromPath(m_tempDir->name() + "tikztemplate.tex");

	if (url.isValid() && !url.isLocalFile() && KIO::NetAccess::exists(url, KIO::NetAccess::SourceSide, m_parentWidget))
	{
		KIO::Job *job = KIO::file_copy(url, localUrl, -1, KIO::Overwrite | KIO::HideProgressInfo);
		if (!KIO::NetAccess::synchronousRun(job, m_parentWidget))
		{
//			KMessageBox::information(m_parentWidget, i18nc("@info", "Template file could not be copied to a temporary file <filename>%1</filename>.", localUrl.prettyUrl()));
			KMessageBox::information(m_parentWidget, tr("Template file could not be copied to a temporary file \"%1\".").arg(localUrl.prettyUrl()));
			return false;
		}
		m_tikzPreviewGenerator->setTemplateFile(localUrl.path());
	}
	else
#endif
		m_tikzPreviewGenerator->setTemplateFile(path);
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
	QAction *action = qobject_cast<QAction*>(sender());
	bool templateChanged = (action == 0) ? true : false; // XXX dirty hack: the template hasn't changed when the Build button in the app has been pressed (if available), the other cases in which this function is called is when a file is opened, in which case everything should be cleaned up and regenerated
	generatePreview(templateChanged);
}

void TikzPreviewController::generatePreview(bool templateChanged)
{
	if (templateChanged) // old aux files may contain commands available in the old template, but not anymore in the new template
		cleanUp();
	// TODO: m_tikzPreviewGenerator->addToTexinputs(QFileInfo(m_mainWidget->url().path()).absolutePath());
//	m_tikzPreviewGenerator->setTikzFilePath(m_mainWidget->url().path()); // the directory in which the pgf file is located is added to TEXINPUTS before running latex
	m_tikzPreviewGenerator->generatePreview(templateChanged);
}

void TikzPreviewController::regeneratePreview()
{
//	m_tikzPreviewGenerator->setTikzFilePath(m_mainWidget->url().path()); // the directory in which the pgf file is located is added to TEXINPUTS before running latex
	m_tikzPreviewGenerator->regeneratePreview();
}

void TikzPreviewController::emptyPreview()
{
	setExportActionsEnabled(false);
	m_tikzPreviewGenerator->abortProcess(); // abort still running processes
	m_tikzPreview->emptyPreview();
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
}

void TikzPreviewController::setExportActionsEnabled(bool enabled)
{
	m_exportAction->setEnabled(enabled);
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

/***************************************************************************/

bool TikzPreviewController::cleanUp()
{
	bool success = true;

	const QFileInfo tempTikzFileInfo(m_tempTikzFileBaseName + ".tex");
	QDir tempTikzDir(tempTikzFileInfo.absolutePath());
	QStringList filters;
	filters << tempTikzFileInfo.completeBaseName() + ".*";

	foreach (const QString &fileName, tempTikzDir.entryList(filters))
		success = success && tempTikzDir.remove(fileName);
	return success;
}
