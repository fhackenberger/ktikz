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

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextEdit>
#include <QTextStream>

#include "tikzpngpreviewer.h"

TikzPngPreviewer::TikzPngPreviewer(const QTextEdit* tikzTextEdit)
    : m_minUpdateInterval(0, 0, 1 /*sec*/, 0)
{
	m_tikzTextEdit = tikzTextEdit;
	m_tikzPdfDoc = 0;
	m_tikzTextEditEmpty = true;
	m_updateScheduled = false;
	m_runFailed = false;
	m_keepRunning = true;
	m_process = 0;
	m_latexCommand = "pdflatex";
	m_pdftopsCommand = "pdftops";
	m_useShellEscaping = false;
	start();
}

TikzPngPreviewer::~TikzPngPreviewer()
{
	// stop the thread before destroying this object
	m_keepRunning = false;
	m_updateRequested.wakeAll();
	wait();
	emit processRunning(false);

	if (m_tikzPdfDoc)
		delete m_tikzPdfDoc;

	// removing all temporary files
	qDebug() << "removing tempfiles";
	if (!m_tikzTempFileBaseName.isEmpty() && !cleanUp())
		qCritical() << "Error: removing tempfiles failed";
}

void TikzPngPreviewer::setLatexCommand(const QString &command)
{
	m_latexCommand = command;
}

void TikzPngPreviewer::setPdftopsCommand(const QString &command)
{
	m_pdftopsCommand = command;
}

void TikzPngPreviewer::setShellEscaping(bool useShellEscaping)
{
	m_useShellEscaping = useShellEscaping;
}

void TikzPngPreviewer::setTemplateFile(const QString &fileName)
{
	m_templateFileName = fileName;
//	createTempLatexFile(); // for some obscure reason, this does not work
	m_templateChanged = true;
}

void TikzPngPreviewer::setTemplateFileAndRegenerate(const QString &fileName)
{
	setTemplateFile(fileName);
	regeneratePreview();
}

void TikzPngPreviewer::setReplaceText(const QString &replace)
{
	m_tikzReplaceText = replace;
}

QString TikzPngPreviewer::getLogText() const
{
	const QMutexLocker lock(&m_memberLock);
	return m_logText;
}

QString TikzPngPreviewer::getParsedLogText(QTextStream *logStream) const
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
				if (!logLine.startsWith("Type")) // don't add lines that invite the user to type a command, since we are not in the console
					logText += logLine;
				logLine = logStream->readLine();
			}
			logText += "\n";
			if (logStream->atEnd()) break;

			// add the line starting with "l.<number> ..." and the next line
			lineNum = QString::number(rx.cap(1).toInt() - 7);
			logLine = "l." + lineNum + rx.cap(2);
			logText += logLine + "\n";
			logText += logStream->readLine() + "\n";
		}
		else
		{
			for (int i = 1; i < keywordPatterns.size(); ++i)
			{
				if (logLine.contains(keywordPatterns.at(i)))
				{
					logText += logLine + "\n";
					logText += logStream->readLine() + "\n";
					logText += logStream->readLine() + "\n";
					break;
				}
			}
		}
	}

	return logText;
}

void TikzPngPreviewer::parseLogFile()
{
	const QFileInfo latexLogFileInfo = QFileInfo(m_tikzTempFileBaseName + ".log");
	QFile latexLogFile(latexLogFileInfo.absoluteFilePath());
	if (!latexLogFile.open(QFile::ReadOnly))
	{
		if (!m_tikzTextEditEmpty)
		{
			m_shortLogText += "\n[LaTeX] " + tr("Warning: could not load LaTeX logfile.");
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
		if (m_runFailed && !m_shortLogText.contains("Process aborted"))
			m_shortLogText = getParsedLogText(&latexLog);
		latexLog.seek(0);
		m_logText += latexLog.readAll();
		latexLogFile.close();
		m_memberLock.unlock();
	}
	emit shortLogUpdated(m_shortLogText, m_runFailed);
//	emit logUpdated(m_runFailed);
}

bool TikzPngPreviewer::hasRunFailed()
{
	const QMutexLocker lock(&m_memberLock);
	return m_runFailed;
}

void TikzPngPreviewer::run()
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
				int msecsToSleep = msecUpdateInterval - m_updateTimer.elapsed();
				m_memberLock.unlock();
				msleep(msecsToSleep);
				continue;
			}
			m_tikzTextEditEmpty = false;
			m_runFailed = false;
			m_updateScheduled = false;
			createTempTikzFile();
			m_memberLock.unlock();
			m_logText = "";
			if (generatePdfFile())
			{
				const QFileInfo tikzPdfFileInfo(m_tikzTempFileBaseName + ".pdf");
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
					}
					else
					{
						m_shortLogText = "[LaTeX] " + tr("Error: loading PDF failed, the file is probably corrupted.");
						qWarning() << "Error: loading PDF failed, the file is probably corrupted:" << qPrintable(tikzPdfFileInfo.absoluteFilePath());
					}
				}
			}
			parseLogFile();
			m_memberLock.lock();
		}
		else
			m_updateRequested.wait(&m_memberLock);
		m_memberLock.unlock();
	}
}

void TikzPngPreviewer::createTempLatexFile()
{
	cleanUp();
	QTemporaryFile tikzTempFile(QDir::tempPath() + "/ktikz/ktikzXXXXXX.tex");
	tikzTempFile.open();
	tikzTempFile.setAutoRemove(false);

	const QFileInfo tikzTempFileInfo = QFileInfo(tikzTempFile);
	m_tikzTempFileBaseName = tikzTempFileInfo.absolutePath() + "/" + tikzTempFileInfo.completeBaseName();
	const QString inputTikzCode = "\\input{" + m_tikzTempFileBaseName + ".pgf}";

	QTextStream tikzStream(&tikzTempFile);

	QFile templateFile(m_templateFileName);
	if (templateFile.open(QIODevice::ReadOnly)) // if user-specified template file is readable
	{
		QTextStream templateFileStream(&templateFile);
		QString templateLine;
		for (int i = 1; !templateFileStream.atEnd(); ++i)
		{
			templateLine = templateFileStream.readLine();
			if (templateLine.indexOf(m_tikzReplaceText) >= 0)
			{
				templateLine.replace(m_tikzReplaceText, inputTikzCode);
				m_templateStartLineNumber = i;
			}
			tikzStream << templateLine << '\n';
		}
	}
	else // use our own template
	{
		tikzStream << "\\documentclass[12pt]{article}\n"
		    << "\\usepackage{tikz}\n"
		    << "\\usepackage{pgf}\n"
		    << "\\usepackage[active,pdftex,tightpage]{preview}\n"
		    << "\\PreviewEnvironment[]{tikzpicture}\n"
			<< "\\begin{document}\n"
			<< inputTikzCode + "\n"
			<< "\\end{document}\n";
		m_templateStartLineNumber = 7;
	}

	qDebug() << "latex code written to:" << qPrintable(QFileInfo(tikzTempFile).absoluteFilePath());
}

void TikzPngPreviewer::createTempTikzFile()
{
	if (m_tikzTextEdit->document()->isEmpty()) // avoid that the previous picture is still displayed
	{
		m_tikzTextEditEmpty = true;
		return;
	}

	if (m_templateChanged)
	{
		createTempLatexFile();
		m_templateChanged = false;
	}

	QFile tikzTempTikzFile(m_tikzTempFileBaseName + ".pgf");
	if (tikzTempTikzFile.open(QIODevice::WriteOnly))
	{
		QTextStream tikzStream(&tikzTempTikzFile);
		tikzStream << m_tikzTextEdit->toPlainText() << endl;
		tikzTempTikzFile.close();
		qDebug() << "tikz code written to:" << qPrintable(QFileInfo(tikzTempTikzFile).absoluteFilePath());
	}
}

bool TikzPngPreviewer::runProcess(const QString &name, const QString &command, const QStringList &arguments, const QString &workingDir)
{
	/* setting the font of a QListWidgetItem in TikzCommandInserter::addListWidgetItems
	 * causes the program to hang here with a "Xlib: unexpected async reply" when inserting text in the textEdit;
	 * the following lock seems to solve this problem (if the lock comes before the setOverrideCursor) */
	m_memberLock.lock();
	m_process = new QProcess;
	m_processAborted = false;
	if (!workingDir.isEmpty())
		m_process->setWorkingDirectory(workingDir);

	if (!m_templateFileName.isEmpty())
	{
		QFileInfo templateFileInfo(m_templateFileName);
		QStringList env = QProcess::systemEnvironment();
		QRegExp rx("^TEXINPUTS=(.*)", Qt::CaseInsensitive);
		if (env.indexOf(rx) >= 0)
#ifdef Q_OS_WIN
			env.replaceInStrings(rx, "TEXINPUTS=\\1;" + templateFileInfo.absolutePath() + ";");
#else
			env.replaceInStrings(rx, "TEXINPUTS=\\1:" + templateFileInfo.absolutePath() + ":");
#endif
		else
#ifdef Q_OS_WIN
			env << "TEXINPUTS=;" + templateFileInfo.absolutePath() + ";";
#else
			env << "TEXINPUTS=:" + templateFileInfo.absolutePath() + ":";
#endif
		m_process->setEnvironment(env);
	}

	m_process->start(command, arguments);
	m_memberLock.unlock();  // we must unlock here so that the signal emitted above can reach the main thread
	emit processRunning(true);
	if (!m_process->waitForStarted(1000)) m_runFailed = true;
	qDebug() << "starting" << command + " " + arguments.join(" ");

	QByteArray buffer;
	QTextStream log(&buffer);
	while (m_process->state() != QProcess::NotRunning)
	{
		// still running or starting
		if (m_process->bytesAvailable())
			log << m_process->readLine(m_process->bytesAvailable());
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
		m_shortLogText = "[" + name + "] " + tr("Process aborted.");
		m_runFailed = true;
	}
	else if (m_process->exitCode() == 0)
	{
		m_shortLogText = "[" + name + "] " + tr("Process finished successfully.");
		m_runFailed = false;
	}
	else
	{
		m_shortLogText = "[" + name + "] " + tr("Error: run failed.");
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

void TikzPngPreviewer::abortProcess()
{
	if (m_process)
	{
		m_process->kill();
		m_processAborted = true;
	}
}

bool TikzPngPreviewer::generateEpsFile()
{
	QStringList pdftopsArguments;
	pdftopsArguments << m_tikzTempFileBaseName + ".pdf" << m_tikzTempFileBaseName + ".eps";
	return runProcess("pdftops", m_pdftopsCommand, pdftopsArguments);
}

bool TikzPngPreviewer::generatePdfFile()
{
	// remove log file before running pdflatex again
	QDir::root().remove(m_tikzTempFileBaseName + ".log");

	if (m_tikzTextEditEmpty) // if the text edit is empty, then clean up files and preview
	{
		QDir::root().remove(m_tikzTempFileBaseName + ".pdf");
		if (m_tikzPdfDoc)
			delete m_tikzPdfDoc;
		m_tikzPdfDoc = 0;
		emit pixmapUpdated(m_tikzPdfDoc);
		return false;
	}

	QStringList latexArguments;
	if (m_useShellEscaping)
		latexArguments << "-shell-escape";
	latexArguments << "-halt-on-error" << "-file-line-error"
	    << "-interaction" << "batchmode" << "-output-directory"
	    << QFileInfo(m_tikzTempFileBaseName + ".tex").absolutePath()
	    << m_tikzTempFileBaseName + ".tex";

	m_shortLogText = "[LaTeX] " + tr("Running...");
	emit shortLogUpdated(m_shortLogText, m_runFailed);
	return runProcess("LaTeX", m_latexCommand, latexArguments, m_tikzTempFileBaseName + ".tex");
}

void TikzPngPreviewer::regeneratePreview()
{
	const QMutexLocker locker(&m_memberLock);
	m_updateScheduled = true;
	m_updateTimer.start();
	m_updateRequested.wakeAll();
}

bool TikzPngPreviewer::exportImage(const QString &fileName, const QString &type)
{
	QString fileName2 = fileName;
	if (!fileName.endsWith(type))
		fileName2 = fileName + "." + type;

	const QFileInfo tikzPdfFileInfo(m_tikzTempFileBaseName + ".pdf");

	if (type == "eps")
	{
		if (!generateEpsFile()) return false;
		const QFileInfo tikzEpsFileInfo(m_tikzTempFileBaseName + ".eps");
		QFile::remove(fileName2);
		return QFile::copy(tikzEpsFileInfo.absoluteFilePath(), fileName2);
	}
	else if (type == "pdf")
	{
		QFile::remove(fileName2);
		return QFile::copy(tikzPdfFileInfo.absoluteFilePath(), fileName2);
	}
	return false;
}

bool TikzPngPreviewer::cleanUp()
{
	bool success = true;

	const QFileInfo tikzTempFileInfo(m_tikzTempFileBaseName + ".tex");
	QDir tikzTempDir(tikzTempFileInfo.absolutePath());
	QStringList filters;
	filters << tikzTempFileInfo.completeBaseName() + ".*";

	QString fileName;
	foreach (fileName, tikzTempDir.entryList(filters))
		success = success && tikzTempDir.remove(fileName);
	return success;
}
