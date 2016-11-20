/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2014                *
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

#include "tikzpreviewgenerator.h"

#ifdef KTIKZ_USE_KDE
#include <KFileItem>
#endif
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QTextStream>
#include <QtGui/QPixmap>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtCore/QStandardPaths>
#include <QtWidgets/QPlainTextEdit>
#include <poppler-qt5.h>
#else
#include <QtGui/QPlainTextEdit>
#include <poppler-qt4.h>
#endif

#include "tikzpreviewcontroller.h"
#include "mainwidget.h"
#include "utils/file.h"

#ifdef Q_OS_WIN
static const QChar s_pathSeparator = QLatin1Char(';');
#else
static const QChar s_pathSeparator = QLatin1Char(':');
#endif

TikzPreviewGenerator::TikzPreviewGenerator(TikzPreviewController *parent)
	: m_parent(parent)
	, m_tikzPdfDoc(0)
	, m_process(0)
	, m_processAborted(false)
	, m_runFailed(false)
	, m_firstRun(true)
	, m_templateChanged(true) // is set correctly in generatePreviewImpl()
	, m_useShellEscaping(false) // is set in setShellEscaping() at startup
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	, m_checkGnuplotExecutable(0)
#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
{
	qRegisterMetaType<TemplateStatus>("TemplateStatus"); // needed for Q_ARG below

	m_processEnvironment = QProcessEnvironment::systemEnvironment();

	moveToThread(&m_thread);
	m_thread.start();
}

TikzPreviewGenerator::~TikzPreviewGenerator()
{
	// stop the thread before destroying this object
	if (m_thread.isRunning())
	{
		m_thread.quit();
		m_thread.wait();
	}
//	Q_EMIT processRunning(false); // this causes a segmentation fault on exit on Arch Linux

	delete m_tikzPdfDoc;
}

/***************************************************************************/

void TikzPreviewGenerator::setTikzFileBaseName(const QString &name)
{
	const QMutexLocker lock(&m_memberLock);
	m_tikzFileBaseName = name;
}

void TikzPreviewGenerator::setLatexCommand(const QString &command)
{
	const QMutexLocker lock(&m_memberLock);
	m_latexCommand = command;
}

void TikzPreviewGenerator::setPdftopsCommand(const QString &command)
{
	const QMutexLocker lock(&m_memberLock);
	m_pdftopsCommand = command;
}

void TikzPreviewGenerator::setShellEscaping(bool useShellEscaping)
{
	m_memberLock.lock();
	m_useShellEscaping = useShellEscaping;
	m_memberLock.unlock();

	if (useShellEscaping)
	{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		QString gnuplotPath = QStandardPaths::findExecutable(QLatin1String("gnuplot")); // search in system path
		if (gnuplotPath.isEmpty() || !QFileInfo(gnuplotPath).isExecutable()) // not found
			Q_EMIT showErrorMessage(tr("Gnuplot cannot be executed.  Either Gnuplot is not installed "
			                           "or it is not available in the system PATH or you may have insufficient "
			                           "permissions to invoke the program."));
#else // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		m_checkGnuplotExecutable = new QProcess;
		m_checkGnuplotExecutable->start(QLatin1String("gnuplot"), QStringList() << QLatin1String("--version"));
//		m_checkGnuplotExecutable->moveToThread(&m_thread);
		connect(m_checkGnuplotExecutable, SIGNAL(error(QProcess::ProcessError)), this, SLOT(displayGnuplotNotExecutable()));
		connect(m_checkGnuplotExecutable, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(checkGnuplotExecutableFinished(int,QProcess::ExitStatus)));
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	}
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void TikzPreviewGenerator::displayGnuplotNotExecutable()
{
	Q_EMIT showErrorMessage(tr("Gnuplot cannot be executed.  Either Gnuplot is not installed "
	                           "or it is not available in the system PATH or you may have insufficient "
	                           "permissions to invoke the program."));
	const QMutexLocker lock(&m_memberLock);
	m_checkGnuplotExecutable->deleteLater();
//	m_checkGnuplotExecutable = 0;
}

void TikzPreviewGenerator::checkGnuplotExecutableFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode)
	Q_UNUSED(exitStatus)
	const QMutexLocker lock(&m_memberLock);
	m_checkGnuplotExecutable->deleteLater();
//	m_checkGnuplotExecutable = 0;
}
#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

void TikzPreviewGenerator::setTemplateFile(const QString &fileName)
{
	m_memberLock.lock();
	const QString oldTemplateFileName = m_templateFileName;
	m_templateFileName = fileName;
	m_memberLock.unlock();

	if (!oldTemplateFileName.isEmpty())
		removeFromLatexSearchPath(QFileInfo(oldTemplateFileName).absolutePath());

	if (!fileName.isEmpty())
		addToLatexSearchPath(QFileInfo(fileName).absolutePath());
}

void TikzPreviewGenerator::setReplaceText(const QString &replace)
{
	const QMutexLocker lock(&m_memberLock);
	m_tikzReplaceText = replace;
}

/***************************************************************************/

QString TikzPreviewGenerator::getLogText() const
{
	const QMutexLocker lock(&m_memberLock);
	return m_logText;
}

static QString getParsedLogText(QTextStream *logStream)
{
	QString logText;

	QRegExp errorPattern(QLatin1String("(\\S*):(\\d+): (.*$)"));
	QList<QLatin1String> errorMessageList;
	errorMessageList << QLatin1String("Undefined control sequence")
	                 << QLatin1String("LaTeX Warning:") << QLatin1String("LaTeX Error:")
	                 << QLatin1String("Runaway argument?")
	                 << QLatin1String("Missing character:") << QLatin1String("Error:");

	QString logLine;
	while (!logStream->atEnd())
	{
		logLine = logStream->readLine();
		if (errorPattern.indexIn(logLine) > -1)
		{
			// show error message and correct line number
			QString lineNum = QString::number(errorPattern.cap(2).toInt());
			const QString errorMsg = errorPattern.cap(3);
			logText += QLatin1String("[LaTeX] Line ") + lineNum + QLatin1String(": ") + errorMsg;

			// while we don't get a line starting with "l.<number> ...", we have to add the line to the first error message
			QRegExp rx(QLatin1String("^l\\.(\\d+)(.*)"));
			logLine = logStream->readLine();
			while (rx.indexIn(logLine) < 0 && !logStream->atEnd())
			{
				if (logLine.isEmpty())
					logText += QLatin1String("\n[LaTeX] Line ") + lineNum + QLatin1String(": ");
				if (!logLine.startsWith(QLatin1String("Type"))) // don't add lines that invite the user to type a command, since we are not in the console
					logText += logLine;
				logLine = logStream->readLine();
			}
			logText += QLatin1Char('\n');
			if (logStream->atEnd()) break;

			// add the line starting with "l.<number> ..." and the next line
			lineNum = QString::number(rx.cap(1).toInt() - 7);
			logLine = QLatin1String("l.") + lineNum + rx.cap(2);
			logText += logLine + QLatin1Char('\n');
			logText += logStream->readLine() + QLatin1Char('\n');
		}
		else
		{
			for (int i = 1; i < errorMessageList.size(); ++i)
			{
				if (logLine.contains(errorMessageList.at(i)))
				{
					logText += logLine + QLatin1Char('\n');
					logText += logStream->readLine() + QLatin1Char('\n');
					logText += logStream->readLine() + QLatin1Char('\n'); // we assume that the error message is not displayed on more than 3 lines in the log, so we stop here
					break;
				}
			}
		}
	}

	return logText;
}

void TikzPreviewGenerator::parseLogFile()
{
	const QMutexLocker lock(&m_memberLock);
	QString longLogText;
	const QString latexLogFilePath = QFileInfo(m_tikzFileBaseName + QLatin1String(".log")).absoluteFilePath();
	QFile latexLogFile(latexLogFilePath);
	if (!latexLogFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		if (!m_tikzCode.isEmpty())
		{
			longLogText = QLatin1String("\n[LaTeX] ") + tr("Warning: could not load LaTeX log file.", "info process");
			m_shortLogText += longLogText;
			longLogText += tr("\nLog file: %1", "info process").arg(latexLogFilePath);
			Q_EMIT showErrorMessage(m_shortLogText);
			Q_EMIT appendLog(longLogText, m_runFailed);
		}
		else
		{
qCritical("does that ever happen?");
			m_shortLogText.clear();
			m_logText.clear();
			Q_EMIT updateLog(QString(), m_runFailed);
		}
	}
	else
	{
		QTextStream latexLog(&latexLogFile);
		if (m_runFailed && !m_shortLogText.contains(tr("Process aborted.")))
		{
			longLogText = getParsedLogText(&latexLog);
			Q_EMIT updateLog(longLogText, m_runFailed);
		}
		latexLog.seek(0);
		m_logText += latexLog.readAll();
		latexLogFile.close();
	}
}

/***************************************************************************/

static QList<qreal> tikzCoordinates(const QString &tikzFileBaseName)
{
	QList<qreal> tikzCoordinateList;
	const QFileInfo tikzAuxFileInfo = QFileInfo(tikzFileBaseName + QLatin1String(".ktikzaux"));
	QFile tikzAuxFile(tikzAuxFileInfo.absoluteFilePath());
	if (tikzAuxFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		QTextStream tikzAuxFileStream(&tikzAuxFile);
		while (!tikzAuxFileStream.atEnd())
		{
			QStringList tikzCoordinateStringList = tikzAuxFileStream.readLine().split(QLatin1Char(';'));
			Q_FOREACH (const QString &tikzCoordinateString, tikzCoordinateStringList)
				tikzCoordinateList << tikzCoordinateString.toDouble();
		}
	}
	return tikzCoordinateList;
}

static QString createTempLatexFile(const QString &tikzFileBaseName, const QString &templateFileName, const QString &tikzReplaceText, const TextCodecProfile *codecProfile);
static QString createTempTikzFile(const QString &tikzFileBaseName, const QString &tikzCode, const TextCodecProfile *codecProfile);

void TikzPreviewGenerator::createPreview()
{
	// avoid that the user can export to a file while the preview is being generated
	Q_EMIT setExportActionsEnabled(false);

	// avoid that the previous picture is still displayed
	m_memberLock.lock();
	if (m_tikzCode.isEmpty())
	{
		m_memberLock.unlock();
		return;
	}

	// load template file if changed
	if (m_templateChanged)
	{
		const QString errorString = createTempLatexFile(m_tikzFileBaseName, m_templateFileName, m_tikzReplaceText, m_parent->textCodecProfile());
		if (!errorString.isEmpty())
		{
			showFileWriteError(m_tikzFileBaseName + QLatin1String(".tex"), errorString);
			m_memberLock.unlock();
			return;
		}
		m_templateChanged = false;
	}

	// load tikz code
	const QString errorString = createTempTikzFile(m_tikzFileBaseName, m_tikzCode, m_parent->textCodecProfile());
	if (!errorString.isEmpty())
	{
		showFileWriteError(m_tikzFileBaseName + QLatin1String(".pgf"), errorString);
		m_memberLock.unlock();
		return;
	}

	// compile everything, show preview and parse log
	m_logText.clear();
	m_memberLock.unlock();
	if (generatePdfFile(m_tikzFileBaseName, m_latexCommand, m_useShellEscaping))
	{
		m_memberLock.lock();
		const QFileInfo tikzPdfFileInfo(m_tikzFileBaseName + QLatin1String(".pdf"));
		if (!tikzPdfFileInfo.exists())
			qWarning() << "Error:" << qPrintable(tikzPdfFileInfo.absoluteFilePath()) << "does not exist";
		else
		{
			// Update widget
			if (m_tikzPdfDoc)
				delete m_tikzPdfDoc;
			m_tikzPdfDoc = Poppler::Document::load(tikzPdfFileInfo.absoluteFilePath());
			if (m_tikzPdfDoc)
			{
				m_shortLogText = QLatin1String("[LaTeX] ") + tr("Process finished successfully.", "info process");
				Q_EMIT pixmapUpdated(m_tikzPdfDoc, tikzCoordinates(m_tikzFileBaseName));
				Q_EMIT setExportActionsEnabled(true);
			}
			else
			{
				m_shortLogText = QLatin1String("[LaTeX] ") + tr("Error: loading PDF failed, the file is probably corrupted.", "info process");
				Q_EMIT showErrorMessage(m_shortLogText);
				Q_EMIT updateLog(m_shortLogText + tr("\nPDF file: %1").arg(tikzPdfFileInfo.absoluteFilePath()), m_runFailed);
			}
		}
		m_memberLock.unlock();
	}
	parseLogFile();
}

/***************************************************************************/

bool TikzPreviewGenerator::hasRunFailed()
{
	const QMutexLocker lock(&m_memberLock);
	return m_runFailed;
}

void TikzPreviewGenerator::generatePreview(TemplateStatus templateStatus)
{
	// Dirty hack because calling generatePreviewImpl directly from the main
	// thread runs it in the main thread (only when triggered by a signal,
	// it is run in the new thread).
	// Note that abortProcess() must be run in the main thread; it kills
	// previous calls to generatePreviewImpl() so that there is no
	// interference between consecutive calls.
	abortProcess();
	QMetaObject::invokeMethod(this, "generatePreviewImpl", Q_ARG(TemplateStatus, templateStatus));
}

void TikzPreviewGenerator::generatePreviewImpl(TemplateStatus templateStatus)
{
	m_memberLock.lock();
	// Each time the tikz code is edited TikzPreviewController->regeneratePreview()
	// is run, which runs generatePreview(DontReloadTemplate). This is OK
	// in all cases, except at startup, because then the template is not yet
	// copied to the temporary directory, so in order to make this happen,
	// m_templateChanged should be set to true.
	if (m_firstRun)
	{
		m_templateChanged = true;
		m_firstRun = false;
	}
	else
		m_templateChanged = (templateStatus == ReloadTemplate);
	m_tikzCode = m_parent->tikzCode();
	m_runFailed = false;
	m_memberLock.unlock();
	createPreview();
}

/***************************************************************************/

void TikzPreviewGenerator::showFileWriteError(const QString &fileName, const QString &errorMessage)
{
	const QString error = tr("Cannot write file \"%1\":\n%2").arg(fileName).arg(errorMessage);
	Q_EMIT showErrorMessage(error);
	Q_EMIT updateLog(error, true);
}

static QString createTempLatexFile(const QString &tikzFileBaseName, const QString &templateFileName, const QString &tikzReplaceText, const TextCodecProfile *codecProfile)
{
	const QString inputTikzCode = QLatin1String("\\makeatletter\n"
		"\\ifdefined\\endtikzpicture%\n"
		"  \\newdimen\\ktikzorigx\n"
		"  \\newdimen\\ktikzorigy\n"
		"  \\newwrite\\ktikzauxfile\n"
		"  \\immediate\\openout\\ktikzauxfile\\jobname.ktikzaux\n"
		"  \\let\\oldendtikzpicture\\endtikzpicture\n"
		"  \\def\\endtikzpicture{%\n"
		"    \\pgfextractx{\\ktikzorigx}{\\pgfpointxy{1}{0}}\n"
		"    \\pgfextracty{\\ktikzorigy}{\\pgfpointxy{0}{1}}\n"
		"    \\pgfmathsetmacro{\\ktikzunitx}{\\ktikzorigx}\n"
		"    \\pgfmathsetmacro{\\ktikzunity}{\\ktikzorigy}\n"
		"    \\pgfmathsetmacro{\\ktikzminx}{\\csname pgf@picminx\\endcsname}\n"
		"    \\pgfmathsetmacro{\\ktikzmaxx}{\\csname pgf@picmaxx\\endcsname}\n"
		"    \\pgfmathsetmacro{\\ktikzminy}{\\csname pgf@picminy\\endcsname}\n"
		"    \\pgfmathsetmacro{\\ktikzmaxy}{\\csname pgf@picmaxy\\endcsname}\n"
		"    \\immediate\\write\\ktikzauxfile{\\ktikzunitx;\\ktikzunity;\\ktikzminx;\\ktikzmaxx;\\ktikzminy;\\ktikzmaxy}\n"
		"    \\oldendtikzpicture\n"
		"  }\n"
		"\\fi\n"
		"\\makeatother"
#ifdef Q_OS_WIN32
		"\\input{") + QFileInfo(tikzFileBaseName).baseName() + QLatin1String(".pgf}"
#else
		"\\input{") + tikzFileBaseName + QLatin1String(".pgf}"
#endif
		"\\makeatletter\n"
		"\\ifdefined\\endtikzpicture%\n"
		"  \\immediate\\closeout\\ktikzauxfile\n"
		"\\fi\n"
		"\\makeatother");

	File tikzTexFile(tikzFileBaseName + QLatin1String(".tex"), File::WriteOnly);
	if (!tikzTexFile.open())
		return tikzTexFile.errorString();

	QTextStream tikzStream(tikzTexFile.file());
	codecProfile->configureStreamEncoding(tikzStream);

	QFile templateFile(templateFileName);
#ifdef KTIKZ_USE_KDE
	KFileItem templateFileItem(KFileItem::Unknown, KFileItem::Unknown, KUrl::fromPath(templateFileName));
	if (templateFileItem.determineMimeType()->parentMimeTypes().contains(QLatin1String("text/plain"))
#else
	if (QFileInfo(templateFile).isFile()
#endif
			&& templateFile.open(QIODevice::ReadOnly | QIODevice::Text) // if user-specified template file is readable
			&& !tikzReplaceText.isEmpty())
	{
		QTextStream templateFileStream(&templateFile);
		codecProfile->configureStreamDecoding(templateFileStream);
		while (!templateFileStream.atEnd())
		{
			QString templateLine = templateFileStream.readLine();
			if (templateLine.indexOf(tikzReplaceText) >= 0)
				templateLine.replace(tikzReplaceText, inputTikzCode);
			tikzStream << templateLine << QLatin1Char('\n');
		}
	}
	else // use our own template
	{
		tikzStream << QLatin1String("\\documentclass[12pt]{article}\n"
		              "\\usepackage{tikz}\n"
		              "\\usepackage{pgf}\n"
		              "\\usepackage[active,tightpage]{preview}\n"
		              "\\PreviewEnvironment[]{tikzpicture}\n"
		              "\\PreviewEnvironment[]{pgfpicture}\n"
		              "\\begin{document}\n")
		           << inputTikzCode << QLatin1Char('\n')
		           << QLatin1String("\\end{document}\n");
	}

	tikzStream.flush();

	if (!tikzTexFile.close())
		return tikzTexFile.errorString();

	qDebug() << "latex code written to:" << tikzFileBaseName + QLatin1String(".tex");
	return QString();
}

static QString createTempTikzFile(const QString &tikzFileBaseName, const QString &tikzCode, const TextCodecProfile *codecProfile)
{
	File tikzFile(tikzFileBaseName + QLatin1String(".pgf"), File::WriteOnly);
	if (!tikzFile.open())
		return tikzFile.errorString();

	QTextStream tikzStream(tikzFile.file());
	codecProfile->configureStreamEncoding(tikzStream);

	tikzStream << tikzCode << endl;
	tikzStream.flush();

	if (!tikzFile.close())
		return tikzFile.errorString();

	qDebug() << "tikz code written to:" << tikzFileBaseName + QLatin1String(".pgf");
	return QString();
}

/***************************************************************************/

void TikzPreviewGenerator::addToLatexSearchPath(const QString &path)
{
	const QMutexLocker lock(&m_memberLock);
	const QString texinputsValue = m_processEnvironment.value(QLatin1String("TEXINPUTS"));
	const QString pathWithSeparator = path + s_pathSeparator;
	if (!texinputsValue.contains(pathWithSeparator))
		m_processEnvironment.insert(QLatin1String("TEXINPUTS"), pathWithSeparator + texinputsValue);
}

void TikzPreviewGenerator::removeFromLatexSearchPath(const QString &path)
{
	const QMutexLocker lock(&m_memberLock);
	QString texinputsValue = m_processEnvironment.value(QLatin1String("TEXINPUTS"));
	const QString pathWithSeparator = path + s_pathSeparator;
	if (texinputsValue.contains(pathWithSeparator))
		m_processEnvironment.insert(QLatin1String("TEXINPUTS"), texinputsValue.remove(pathWithSeparator));
}

bool TikzPreviewGenerator::runProcess(const QString &name, const QString &command,
									  const QStringList &arguments, const QString &workingDir)
{
	QString shortLogText;
	QString longLogText;
	bool runFailed = false;

	// Initialize process
	m_memberLock.lock();
	m_process = new QProcess;
	m_processAborted = false;
	if (!workingDir.isEmpty())
		m_process->setWorkingDirectory(workingDir);
	m_process->setProcessEnvironment(m_processEnvironment);

	// Start process
	m_process->start(command, arguments);
	m_memberLock.unlock(); // the following must not be protected by the mutex because we must be able to kill m_process
	Q_EMIT processRunning(true);
	if (!m_process->waitForStarted(1000))
		runFailed = true;
	qDebug() << "starting" << command + QLatin1Char(' ') + arguments.join(QLatin1String(" "));

	// Process is running
	QByteArray buffer;
	QTextStream log(&buffer);
	while (m_process->state() != QProcess::NotRunning)
	{
		// still running or starting
		if (m_process->bytesAvailable())
			log << m_process->readLine();
		else
			m_process->waitForFinished(100 /*msec*/);
	}
	// Process finished
	Q_EMIT processRunning(false);
	if (m_process->bytesAvailable())
		log << m_process->readLine(m_process->bytesAvailable());
	log.seek(0);

	// Postprocessing
	m_memberLock.lock();
	if (m_processAborted)
	{
		shortLogText = QLatin1Char('[') + name + QLatin1String("] ") + tr("Process aborted.", "info process");
		longLogText = shortLogText;
		runFailed = true;
	}
	else if (runFailed) // if the process could not be started
	{
		shortLogText = QLatin1Char('[') + name + QLatin1String("] ") + tr("Error: the process could not be started.", "info process");
		longLogText = shortLogText + tr("\nCommand: %1", "info process").arg(command + QLatin1Char(' ') + arguments.join(QLatin1String(" ")));
	}
	else if (m_process->exitCode() == 0)
	{
		shortLogText = QLatin1Char('[') + name + QLatin1String("] ") + tr("Process finished successfully.", "info process");
		longLogText = shortLogText;
		runFailed = false;
	}
	else
	{
		shortLogText = QLatin1Char('[') + name + QLatin1String("] ") + tr("Error: run failed.", "info process");
		longLogText = shortLogText + tr("\nCommand: %1", "info process").arg(command + QLatin1Char(' ') + arguments.join(QLatin1String(" ")));
		qWarning() << "Error:" << qPrintable(command) << "run failed with exit code:" << m_process->exitCode();
		m_logText = log.readAll();
		runFailed = true;
	}
	delete m_process;
	m_process = 0;
	m_shortLogText = shortLogText;
	m_runFailed = runFailed;
	m_memberLock.unlock();

	if (runFailed)
		Q_EMIT showErrorMessage(shortLogText);
	Q_EMIT updateLog(longLogText, runFailed);

	return !runFailed;
}

void TikzPreviewGenerator::abortProcess()
{
	if (m_process)
	{
		m_process->kill();
		m_processAborted = true;
	}
}

/***************************************************************************/

bool TikzPreviewGenerator::generateEpsFile(int page)
{
	QStringList pdftopsArguments;
	pdftopsArguments << QLatin1String("-f") << QString::number(page+1) << QLatin1String("-l") << QString::number(page+1) << QLatin1String("-eps") << m_tikzFileBaseName + QLatin1String(".pdf") << m_tikzFileBaseName + QLatin1String(".eps");
	return runProcess(QLatin1String("pdftops"), m_pdftopsCommand, pdftopsArguments);
/*
	int width = m_tikzPdfDoc->page(page)->pageSize().width();
	int height = m_tikzPdfDoc->page(page)->pageSize().height();

	Poppler::PSConverter *psConverter = m_tikzPdfDoc->psConverter();
	psConverter->setOutputFileName(m_tikzFileBaseName + QLatin1String(".eps"));
	psConverter->setPageList(QList<int>() << page+1);
	psConverter->setPaperWidth(width);
	psConverter->setPaperHeight(height);
	if (psConverter->convert())
	{
		delete psConverter;
		return true;
	}
	delete psConverter;
	return false;
*/
}

bool TikzPreviewGenerator::generatePdfFile(const QString &tikzFileBaseName, const QString &latexCommand, bool useShellEscaping)
{
	// remove log file before running pdflatex again
	QDir::root().remove(tikzFileBaseName + QLatin1String(".log"));

	QStringList latexArguments;
	if (useShellEscaping)
		latexArguments << QLatin1String("-shell-escape");
	latexArguments << QLatin1String("-halt-on-error") << QLatin1String("-file-line-error")
	               << QLatin1String("-interaction") << QLatin1String("nonstopmode") << QLatin1String("-output-directory")
	               << QFileInfo(tikzFileBaseName + QLatin1String(".tex")).absolutePath()
	               << tikzFileBaseName + QLatin1String(".tex");

	Q_EMIT updateLog(QLatin1String("[LaTeX] ") + tr("Running...", "info process"), false); // runFailed = false
	return runProcess(QLatin1String("LaTeX"), latexCommand, latexArguments, QFileInfo(tikzFileBaseName).absolutePath());
}
