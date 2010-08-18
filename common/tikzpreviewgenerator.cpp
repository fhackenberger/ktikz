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
#include <KSaveFile>
#else
#include <QFile>
#endif
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QProcess>
#include <QPlainTextEdit>
#include <QTextStream>

#include <poppler-qt4.h>

#include "tikzpreviewcontroller.h"

const QTime s_standardMinUpdateInterval(0, 0, 1 /*sec*/, 0);

TikzPreviewGenerator::TikzPreviewGenerator(TikzPreviewController *parent)
    : m_minUpdateInterval(s_standardMinUpdateInterval)
{
	m_parent = parent;
	m_tikzPdfDoc = 0;
	m_updateScheduled = false;
	m_runFailed = false;
	m_keepRunning = true;
	m_process = 0;
	m_templateChanged = true;
//	m_latexCommand = "pdflatex";
//	m_pdftopsCommand = "pdftops";
//	m_useShellEscaping = false;
	start();
}

TikzPreviewGenerator::~TikzPreviewGenerator()
{
	// stop the thread before destroying this object
	m_keepRunning = false;
	m_updateRequested.wakeAll();
	wait();
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
		QProcess *m_checkGnuplotExecutable = new QProcess(this);
		m_checkGnuplotExecutable->start("gnuplot", QStringList() << "--version");
		connect(m_checkGnuplotExecutable, SIGNAL(error(QProcess::ProcessError)), this, SLOT(displayGnuplotNotExecutable()));
	}
}

void TikzPreviewGenerator::displayGnuplotNotExecutable()
{
	emit showErrorMessage(tr("Gnuplot cannot be executed.  Either Gnuplot is not installed "
	    "or it is not available in the system PATH or you may have insufficient "
	    "permissions to invoke the program."));
//	m_checkGnuplotExecutable->deleteLater();
}

void TikzPreviewGenerator::setTemplateFile(const QString &fileName)
{
	m_templateFileName = fileName;
}

void TikzPreviewGenerator::setReplaceText(const QString &replace)
{
	m_tikzReplaceText = replace;
}

void TikzPreviewGenerator::setMinUpdateInterval(const QTime &interval)
{
	m_minUpdateInterval = interval;
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

	QList<QRegExp> keywordPatterns;
	keywordPatterns << QRegExp("(\\S*):(\\d+): (.*$)")
	    << QRegExp("Undefined control sequence")
	    << QRegExp("LaTeX Warning:") << QRegExp("LaTeX Error:")
	    << QRegExp("Runaway argument?")
	    << QRegExp("Missing character: .*!") << QRegExp("Error:");

	QString logLine;
	while (!logStream->atEnd())
	{
		logLine = logStream->readLine();
		if (keywordPatterns.at(0).indexIn(logLine) > -1)
		{
			// show error message and correct line number
//			QString lineNum = QString::number(keywordPatterns[0].cap(2).toInt() - m_templateStartLineNumber);
			QString lineNum = QString::number(keywordPatterns[0].cap(2).toInt());
			const QString errorMsg = keywordPatterns[0].cap(3);
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
			for (int i = 1; i < keywordPatterns.size(); ++i)
			{
				if (logLine.contains(keywordPatterns.at(i)))
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
				emit pixmapUpdated(m_tikzPdfDoc);
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

void TikzPreviewGenerator::run()
{
	while (m_keepRunning)
	{
		m_memberLock.lock();
		if (m_updateScheduled) // We shall update the picture
		{
			const long int msecUpdateInterval = m_minUpdateInterval.minute() * 60000 + m_minUpdateInterval.second() * 1000; // We ignore hours
			if (m_updateTimer.isValid() && m_updateTimer.elapsed() < msecUpdateInterval)
			{
				// Our m_minUpdateInterval has not elapsed yet, sleep
				const int msecsToSleep = msecUpdateInterval - m_updateTimer.elapsed();
				m_memberLock.unlock();
				msleep(msecsToSleep);
				continue;
			}
			m_tikzCode = m_parent->tikzCode();
			m_runFailed = false;
			m_updateScheduled = false;
			m_memberLock.unlock();
			createPreview();
			m_memberLock.lock();
			setMinUpdateInterval(s_standardMinUpdateInterval);
		}
		else
			m_updateRequested.wait(&m_memberLock);
		m_memberLock.unlock();
	}
}

void TikzPreviewGenerator::regeneratePreview()
{
	const QMutexLocker locker(&m_memberLock);
	m_updateScheduled = true;
	m_updateTimer.start();
	m_updateRequested.wakeAll();
}

void TikzPreviewGenerator::generatePreview(bool templateChanged)
{
	m_templateChanged = templateChanged;
	setMinUpdateInterval(QTime(0, 0, 0, 0));
	regeneratePreview();
}

/***************************************************************************/

void TikzPreviewGenerator::createTempLatexFile()
{
	const QString inputTikzCode = "\\input{" + m_tikzFileBaseName + ".pgf}";

#ifdef KTIKZ_USE_KDE
	KSaveFile tikzTexFile(m_tikzFileBaseName + ".tex");
	if (!tikzTexFile.open())
#else
	QFile tikzTexFile(m_tikzFileBaseName + ".tex");
	if (!tikzTexFile.open(QIODevice::WriteOnly | QIODevice::Text))
#endif
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".tex (" + qPrintable(QFileInfo(tikzTexFile).absoluteFilePath()) + "):\n" + tikzTexFile.errorString();
		return;
	}

	QTextStream tikzStream(&tikzTexFile);

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

#ifdef KTIKZ_USE_KDE
	if (!tikzTexFile.finalize())
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".tex (" + qPrintable(QFileInfo(tikzTexFile).absoluteFilePath()) + "):\n" + tikzTexFile.errorString();
	}
#endif
	tikzTexFile.close();

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

#ifdef KTIKZ_USE_KDE
	KSaveFile tikzFile(m_tikzFileBaseName + ".pgf");
	if (!tikzFile.open())
#else
	QFile tikzFile(m_tikzFileBaseName + ".pgf");
	if (!tikzFile.open(QIODevice::WriteOnly | QIODevice::Text))
#endif
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".pgf (" + qPrintable(QFileInfo(tikzFile).absoluteFilePath()) + "):\n" + tikzFile.errorString();
		return;
	}

	QTextStream tikzStream(&tikzFile);
	tikzStream << m_tikzCode << endl;
	tikzStream.flush();

#ifdef KTIKZ_USE_KDE
	if (!tikzFile.finalize())
	{
//		KMessageBox::error(this, i18nc("@info", "Cannot write file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Save Error"));
		qDebug() << "Cannot write file " + m_tikzFileBaseName + ".pgf (" + qPrintable(QFileInfo(tikzFile).absoluteFilePath()) + "):\n" + tikzFile.errorString();
	}
#endif
	tikzFile.close();

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

#if QT_VERSION >= 0x040600
	m_processEnvironment = QProcessEnvironment::systemEnvironment();
	m_processEnvironment.insert("TEXINPUTS", path + pathSeparator + m_processEnvironment.value("TEXINPUTS"));
#else
	m_processEnvironment = QProcess::systemEnvironment();
	QRegExp rx("^TEXINPUTS=(.*)", Qt::CaseInsensitive);
	const int num = m_processEnvironment.indexOf(rx);
	if (num >= 0)
		m_processEnvironment[num].replace(rx, "TEXINPUTS=" + path + pathSeparator + "\\1");
	else
		m_processEnvironment << "TEXINPUTS=" + path + pathSeparator;
#endif
}

bool TikzPreviewGenerator::runProcess(const QString &name, const QString &command,
    const QStringList &arguments, const QString &workingDir)
{
	m_memberLock.lock();
	m_process = new QProcess;
	m_processAborted = false;
	if (!workingDir.isEmpty())
		m_process->setWorkingDirectory(workingDir);

	if (!m_templateFileName.isEmpty())
	{
		const QFileInfo templateFileInfo(m_templateFileName);
		addToLatexSearchPath(templateFileInfo.absolutePath());
#if QT_VERSION >= 0x040600
		m_process->setProcessEnvironment(m_processEnvironment);
#else
		m_process->setEnvironment(m_processEnvironment);
#endif
	}

	m_process->start(command, arguments);
	m_memberLock.unlock();  // we must unlock here so that the signal emitted above can reach the main thread
	emit processRunning(true);
	if (!m_process->waitForStarted(1000)) m_runFailed = true;
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

bool TikzPreviewGenerator::generateEpsFile()
{
	QStringList pdftopsArguments;
	pdftopsArguments << "-eps" << m_tikzFileBaseName + ".pdf" << m_tikzFileBaseName + ".eps";
	return runProcess("pdftops", m_pdftopsCommand, pdftopsArguments);
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
