/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010 by Glad Deschrijver              *
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

#include "tikzpreviewgenerator.h"

#ifdef KTIKZ_USE_KDE
#include <KFileItem>
#endif
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QProcess>
#include <QPlainTextEdit>
#include <QTextStream>

#include <poppler-qt4.h>

#include "tikzpreviewcontroller.h"
#include "utils/file.h"

TikzPreviewGenerator::TikzPreviewGenerator(TikzPreviewController *parent)
{
	m_parent = parent;
	m_tikzPdfDoc = 0;
	m_runFailed = false;
	m_process = 0;
	m_firstRun = true;

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
	emit processRunning(false);

	if (m_tikzPdfDoc)
		delete m_tikzPdfDoc;
}

/***************************************************************************/

void TikzPreviewGenerator::setTikzFileBaseName(const QString &name)
{
	m_tikzFileBaseName = name;
}

void TikzPreviewGenerator::setLatexCommand(const QString &command)
{
	m_latexCommand = command;
}

void TikzPreviewGenerator::setPdftopsCommand(const QString &command)
{
	m_pdftopsCommand = command;
}

void TikzPreviewGenerator::setShellEscaping(bool useShellEscaping)
{
	m_useShellEscaping = useShellEscaping;

	if (m_useShellEscaping)
	{
		m_checkGnuplotExecutable = new QProcess;
		m_checkGnuplotExecutable->start("gnuplot", QStringList() << "--version");
		m_checkGnuplotExecutable->moveToThread(&m_thread);
		connect(m_checkGnuplotExecutable, SIGNAL(error(QProcess::ProcessError)), this, SLOT(displayGnuplotNotExecutable()));
		connect(m_checkGnuplotExecutable, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(checkGnuplotExecutableFinished(int,QProcess::ExitStatus)));
	}
}

void TikzPreviewGenerator::displayGnuplotNotExecutable()
{
	emit showErrorMessage(tr("Gnuplot cannot be executed.  Either Gnuplot is not installed "
	                         "or it is not available in the system PATH or you may have insufficient "
	                         "permissions to invoke the program."));
	delete m_checkGnuplotExecutable;
	m_checkGnuplotExecutable = 0;
}

void TikzPreviewGenerator::checkGnuplotExecutableFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode)
	Q_UNUSED(exitStatus)
	delete m_checkGnuplotExecutable;
	m_checkGnuplotExecutable = 0;
}

void TikzPreviewGenerator::setTemplateFile(const QString &fileName)
{
	if (!m_templateFileName.isEmpty())
	{
		const QFileInfo templateFileInfo(m_templateFileName);
		removeFromLatexSearchPath(templateFileInfo.absolutePath());
	}

	m_templateFileName = fileName;

	if (!m_templateFileName.isEmpty())
	{
		const QFileInfo templateFileInfo(m_templateFileName);
		addToLatexSearchPath(templateFileInfo.absolutePath());
	}
}

void TikzPreviewGenerator::setReplaceText(const QString &replace)
{
	m_tikzReplaceText = replace;
}

/***************************************************************************/

QString TikzPreviewGenerator::getLogText() const
{
	const QMutexLocker lock(&m_memberLock);
	return m_logText;
}

QString TikzPreviewGenerator::getParsedLogText(QTextStream *logStream) const
{
	QString logText;

	QRegExp errorPattern("(\\S*):(\\d+): (.*$)");
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
//			QString lineNum = QString::number(errorPattern.cap(2).toInt() - m_templateStartLineNumber);
			QString lineNum = QString::number(errorPattern.cap(2).toInt());
			const QString errorMsg = errorPattern.cap(3);
			logText += "[LaTeX] Line " + lineNum + ": " + errorMsg;

			// while we don't get a line starting with "l.<number> ...", we have to add the line to the first error message
			QRegExp rx("^l\\.(\\d+)(.*)");
			logLine = logStream->readLine();
			while (rx.indexIn(logLine) < 0 && !logStream->atEnd())
			{
				if (logLine.isEmpty())
					logText += "\n[LaTeX] Line " + lineNum + ": ";
				if (!logLine.startsWith(QLatin1String("Type"))) // don't add lines that invite the user to type a command, since we are not in the console
					logText += logLine;
				logLine = logStream->readLine();
			}
			logText += '\n';
			if (logStream->atEnd()) break;

			// add the line starting with "l.<number> ..." and the next line
			lineNum = QString::number(rx.cap(1).toInt() - 7);
			logLine = "l." + lineNum + rx.cap(2);
			logText += logLine + '\n';
			logText += logStream->readLine() + '\n';
		}
		else
		{
			for (int i = 1; i < errorMessageList.size(); ++i)
			{
				if (logLine.contains(errorMessageList.at(i)))
				{
					logText += logLine + '\n';
					logText += logStream->readLine() + '\n';
					logText += logStream->readLine() + '\n';
					break;
				}
			}
		}
	}

	return logText;
}

void TikzPreviewGenerator::parseLogFile()
{
	const QFileInfo latexLogFileInfo = QFileInfo(m_tikzFileBaseName + ".log");
	QFile latexLogFile(latexLogFileInfo.absoluteFilePath());
	if (!latexLogFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		if (!m_tikzCode.isEmpty())
		{
			m_shortLogText += "\n[LaTeX] " + tr("Warning: could not load LaTeX logfile.");
			emit showErrorMessage(m_shortLogText);
			qWarning() << "Warning: could not load latex logfile:" << qPrintable(latexLogFileInfo.absoluteFilePath());
		}
		else
		{
			m_shortLogText = "";
			m_logText = "";
		}
	}
	else
	{
		m_memberLock.lock();
		QTextStream latexLog(&latexLogFile);
		if (m_runFailed && !m_shortLogText.contains(tr("Process aborted.")))
			m_shortLogText = getParsedLogText(&latexLog);
		latexLog.seek(0);
		m_logText += latexLog.readAll();
		latexLogFile.close();
		m_memberLock.unlock();
	}
	emit shortLogUpdated(m_shortLogText, m_runFailed);
//	emit logUpdated(m_runFailed);
}

/***************************************************************************/

QList<qreal> TikzPreviewGenerator::tikzCoordinates()
{
	QList<qreal> tikzCoordinateList;
	const QFileInfo tikzAuxFileInfo = QFileInfo(m_tikzFileBaseName + QLatin1String(".ktikzaux"));
	QFile tikzAuxFile(tikzAuxFileInfo.absoluteFilePath());
	if (tikzAuxFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		QTextStream tikzAuxFileStream(&tikzAuxFile);
		QStringList tikzAuxLine;
		for (int i = 0; !tikzAuxFileStream.atEnd(); ++i)
		{
			tikzAuxLine = tikzAuxFileStream.readLine().split(QLatin1Char(';'));
			for (int j = 0; j < tikzAuxLine.length(); ++j)
				tikzCoordinateList << tikzAuxLine.at(j).toDouble();
		}
	}
	return tikzCoordinateList;
}

void TikzPreviewGenerator::createPreview()
{
	emit setExportActionsEnabled(false);
	createTempTikzFile();
	m_logText = "";
	if (generatePdfFile())
	{
		const QFileInfo tikzPdfFileInfo(m_tikzFileBaseName + ".pdf");
		if (!tikzPdfFileInfo.exists())
			qWarning() << "Error:" << qPrintable(tikzPdfFileInfo.absoluteFilePath()) << "does not exists";
		else
		{
			// Update widget
			if (m_tikzPdfDoc)
				delete m_tikzPdfDoc;
			m_tikzPdfDoc = Poppler::Document::load(tikzPdfFileInfo.absoluteFilePath());
			if (m_tikzPdfDoc)
			{
				m_shortLogText = "[LaTeX] " + tr("Process finished successfully.");
				emit pixmapUpdated(m_tikzPdfDoc, tikzCoordinates());
				emit setExportActionsEnabled(true);
			}
			else
			{
				m_shortLogText = "[LaTeX] " + tr("Error: loading PDF failed, the file is probably corrupted.");
				emit showErrorMessage(m_shortLogText);
				qWarning() << "Error: loading PDF failed, the file is probably corrupted:" << qPrintable(tikzPdfFileInfo.absoluteFilePath());
			}
		}
	}
	parseLogFile();
}

/***************************************************************************/

bool TikzPreviewGenerator::hasRunFailed()
{
	const QMutexLocker lock(&m_memberLock);
	return m_runFailed;
}

void TikzPreviewGenerator::generatePreview(bool templateChanged)
{
	// dirty hack because calling generatePreviewImpl directly from the main thread runs it in the main thread (only when triggered by a signal, it is run in the new thread)
	QMetaObject::invokeMethod(this, "generatePreviewImpl", Q_ARG(bool, templateChanged));
}

void TikzPreviewGenerator::generatePreviewImpl(bool templateChanged)
{
	m_memberLock.lock();
	// Each time the tikz code is edited TikzPreviewController->regeneratePreview()
	// is run, which runs generatePreview(false). This is OK in all cases, except
	// at startup, because then the template is not yet copied to the temporary
	// directory, so in order to make this happen, m_templateChanged should be
	// set to true.
	if (m_firstRun)
	{
		m_templateChanged = true;
		m_firstRun = false;
	}
	else
		m_templateChanged = templateChanged;
	m_tikzCode = m_parent->tikzCode();
	m_runFailed = false;
	m_memberLock.unlock();
	createPreview();
}

/***************************************************************************/

void TikzPreviewGenerator::createTempLatexFile()
{
	const QString inputTikzCode = "\\makeatletter\n"
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
		"\\input{" + QFileInfo(m_tikzFileBaseName).baseName() + ".pgf}"
#else
		"\\input{" + m_tikzFileBaseName + ".pgf}"
#endif
		"\\makeatletter\n"
		"\\ifdefined\\endtikzpicture%\n"
		"  \\immediate\\closeout\\ktikzauxfile\n"
		"\\fi\n"
		"\\makeatother";

	File tikzTexFile(m_tikzFileBaseName + ".tex", File::WriteOnly);
	if (!tikzTexFile.open())
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".tex (" + qPrintable(QFileInfo(*tikzTexFile.file()).absoluteFilePath()) + "):\n" + tikzTexFile.errorString();
		return;
	}

	QTextStream tikzStream(tikzTexFile.file());

	QFile templateFile(m_templateFileName);
#ifdef KTIKZ_USE_KDE
	KFileItem templateFileItem(KFileItem::Unknown, KFileItem::Unknown, KUrl::fromPath(m_templateFileName));
	if (templateFileItem.determineMimeType()->parentMimeTypes().contains("text/plain")
#else
	if (QFileInfo(templateFile).isFile()
#endif
	        && templateFile.open(QIODevice::ReadOnly | QIODevice::Text) // if user-specified template file is readable
	        && !m_tikzReplaceText.isEmpty())
	{
		QTextStream templateFileStream(&templateFile);
		QString templateLine;
		for (int i = 1; !templateFileStream.atEnd(); ++i)
		{
			templateLine = templateFileStream.readLine();
			if (templateLine.indexOf(m_tikzReplaceText) >= 0)
			{
				templateLine.replace(m_tikzReplaceText, inputTikzCode);
//				m_templateStartLineNumber = i;
			}
			tikzStream << templateLine << '\n';
		}
	}
	else // use our own template
	{
		tikzStream << "\\documentclass[12pt]{article}\n"
		              "\\usepackage{tikz}\n"
		              "\\usepackage{pgf}\n"
		              "\\usepackage[active,pdftex,tightpage]{preview}\n"
		              "\\PreviewEnvironment[]{tikzpicture}\n"
		              "\\PreviewEnvironment[]{pgfpicture}\n"
		              "\\begin{document}\n"
		              + inputTikzCode + "\n"
		              "\\end{document}\n";
//		m_templateStartLineNumber = 7;
	}

	tikzStream.flush();

	if (!tikzTexFile.close())
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".tex (" + qPrintable(QFileInfo(*tikzTexFile.file()).absoluteFilePath()) + "):\n" + tikzTexFile.errorString();
	}

	qDebug() << "latex code written to:" << m_tikzFileBaseName + ".tex";
}

void TikzPreviewGenerator::createTempTikzFile()
{
	if (m_tikzCode.isEmpty()) // avoid that the previous picture is still displayed
		return;

	if (m_templateChanged)
	{
		createTempLatexFile();
		m_templateChanged = false;
	}

	File tikzFile(m_tikzFileBaseName + ".pgf", File::WriteOnly);
	if (!tikzFile.open())
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".pgf (" + qPrintable(QFileInfo(*tikzFile.file()).absoluteFilePath()) + "):\n" + tikzFile.errorString();
		return;
	}

	QTextStream tikzStream(tikzFile.file());
	tikzStream << m_tikzCode << endl;
	tikzStream.flush();

	if (!tikzFile.close())
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".pgf (" + qPrintable(QFileInfo(*tikzFile.file()).absoluteFilePath()) + "):\n" + tikzFile.errorString();
	}

	qDebug() << "tikz code written to:" << m_tikzFileBaseName + ".pgf";
}

/***************************************************************************/

void TikzPreviewGenerator::addToLatexSearchPath(const QString &path)
{
#ifdef Q_OS_WIN
	const QChar pathSeparator = ';';
#else
	const QChar pathSeparator = ':';
#endif

	const QMutexLocker lock(&m_memberLock);
	const QString texinputsValue = m_processEnvironment.value("TEXINPUTS");
	const QString pathWithSeparator = path + pathSeparator;
	if (!texinputsValue.contains(pathWithSeparator))
		m_processEnvironment.insert("TEXINPUTS", pathWithSeparator + texinputsValue);
}

void TikzPreviewGenerator::removeFromLatexSearchPath(const QString &path)
{
#ifdef Q_OS_WIN
	const QChar pathSeparator = ';';
#else
	const QChar pathSeparator = ':';
#endif

	const QMutexLocker lock(&m_memberLock);
	QString texinputsValue = m_processEnvironment.value("TEXINPUTS");
	const QString pathWithSeparator = path + pathSeparator;
	if (texinputsValue.contains(pathWithSeparator))
		m_processEnvironment.insert("TEXINPUTS", texinputsValue.remove(pathWithSeparator));
}

bool TikzPreviewGenerator::runProcess(const QString &name, const QString &command,
                                      const QStringList &arguments, const QString &workingDir)
{
	m_memberLock.lock();
	m_process = new QProcess;
	m_processAborted = false;
	if (!workingDir.isEmpty())
		m_process->setWorkingDirectory(workingDir);

	m_process->setProcessEnvironment(m_processEnvironment);

	m_process->start(command, arguments);
	m_memberLock.unlock();
	emit processRunning(true);
	if (!m_process->waitForStarted(1000))
		m_runFailed = true;
	qDebug() << "starting" << command + ' ' + arguments.join(" ");

	QByteArray buffer;
	QTextStream log(&buffer);
	while (m_process->state() != QProcess::NotRunning)
	{
		// still running or starting
		if (m_process->bytesAvailable())
//			log << m_process->readLine(m_process->bytesAvailable()); // XXX I don't know why this doesn't work anymore since Qt 4.6
			log << m_process->readLine();
		else
			m_process->waitForFinished(100 /*msec*/);
	}
	// Process finished
	emit processRunning(false);
	if (m_process->bytesAvailable())
		log << m_process->readLine(m_process->bytesAvailable());
	log.seek(0);

	if (m_processAborted)
	{
		m_shortLogText = '[' + name + "] " + tr("Process aborted.");
		emit showErrorMessage(m_shortLogText);
		m_runFailed = true;
	}
	else if (m_runFailed) // if the process could not be started
	{
		m_shortLogText = '[' + name + "] " + tr("Error: the process could not be started", "info process");
		emit showErrorMessage(m_shortLogText);
	}
	else if (m_process->exitCode() == 0)
	{
		m_shortLogText = '[' + name + "] " + tr("Process finished successfully.");
		m_runFailed = false;
	}
	else
	{
		m_shortLogText = '[' + name + "] " + tr("Error: run failed.", "info process");
		emit showErrorMessage(m_shortLogText);
		qWarning() << "Error:" << qPrintable(command) << "run failed with exit code:" << m_process->exitCode();
		m_memberLock.lock();
		m_logText = log.readAll();
		m_memberLock.unlock();
		m_runFailed = true;
	}
	delete m_process;
	m_process = 0;
	emit shortLogUpdated(m_shortLogText, m_runFailed);
	return !m_runFailed;
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
	pdftopsArguments << "-f" << QString::number(page+1) << "-l" << QString::number(page+1) << "-eps" << m_tikzFileBaseName + ".pdf" << m_tikzFileBaseName + ".eps";
	return runProcess("pdftops", m_pdftopsCommand, pdftopsArguments);
/*
	int width = m_tikzPdfDoc->page(page)->pageSize().width();
	int height = m_tikzPdfDoc->page(page)->pageSize().height();

	Poppler::PSConverter *psConverter = m_tikzPdfDoc->psConverter();
	psConverter->setOutputFileName(m_tikzFileBaseName + ".eps");
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

bool TikzPreviewGenerator::generatePdfFile()
{
	// remove log file before running pdflatex again
	QDir::root().remove(m_tikzFileBaseName + ".log");

	if (m_tikzCode.isEmpty()) // if the text edit is empty, then clean up files and preview
	{
		QDir::root().remove(m_tikzFileBaseName + ".pdf");
		if (m_tikzPdfDoc)
			delete m_tikzPdfDoc;
		m_tikzPdfDoc = 0;
		emit pixmapUpdated(0);
		return false;
	}

	QStringList latexArguments;
	if (m_useShellEscaping)
		latexArguments << "-shell-escape";
	latexArguments << "-halt-on-error" << "-file-line-error"
	               << "-interaction" << "nonstopmode" << "-output-directory"
	               << QFileInfo(m_tikzFileBaseName + ".tex").absolutePath()
	               << m_tikzFileBaseName + ".tex";

	m_shortLogText = "[LaTeX] " + tr("Running...");
	emit shortLogUpdated(m_shortLogText, m_runFailed);
	return runProcess("LaTeX", m_latexCommand, latexArguments, QFileInfo(m_tikzFileBaseName).absolutePath());
}
