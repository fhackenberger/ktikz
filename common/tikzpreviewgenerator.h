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

#ifndef KTIKZ_TIKZPREVIEWGENERATOR_H
#define KTIKZ_TIKZPREVIEWGENERATOR_H

#include <QMutex>
#if QT_VERSION >= 0x040600
#include <QProcessEnvironment>
#else
#include <QStringList>
#endif
#include <QThread>
#include <QTime>
#include <QWaitCondition>

class QPixmap;
class QProcess;
class QPlainTextEdit;
class QTextStream;

namespace Poppler
{
	class Document;
}

class TikzPreviewController;

/**
 * @author Florian Hackenberger <florian@hackenberger.at>
 */
class TikzPreviewGenerator : public QThread
{
	Q_OBJECT

public:
	TikzPreviewGenerator(TikzPreviewController *parent);
	~TikzPreviewGenerator();

	void setTikzFileBaseName(const QString &name);
	void setLatexCommand(const QString &command);
	void setPdftopsCommand(const QString &command);
	void setShellEscaping(bool useShellEscaping);
	QString getLogText() const;
	bool hasRunFailed();
	void generatePreview(bool templateChanged = false);
	void addToLatexSearchPath(const QString &path);
	bool generateEpsFile();

public slots:
	void setTemplateFile(const QString &fileName);
	void setReplaceText(const QString &replace);
	void abortProcess();
	void regeneratePreview();

signals:
	void pixmapUpdated(Poppler::Document *tikzPdfDoc);
	void setExportActionsEnabled(bool enabled);
	void showErrorMessage(const QString &message);
	void shortLogUpdated(const QString &logText, bool runFailed);
//	void logUpdated(bool runFailed);
	void processRunning(bool isRunning);

private slots:
	void displayGnuplotNotExecutable();

protected:
	void setMinUpdateInterval(const QTime &interval);
	QString getParsedLogText(QTextStream *logStream) const;
	void parseLogFile();
	void createPreview();
	void run();
	void createTempLatexFile();
	void createTempTikzFile();
	bool runProcess(const QString &name, const QString &command, const QStringList &arguments, const QString &workingDir = 0);
	bool generatePdfFile();

	TikzPreviewController *m_parent;
	Poppler::Document *m_tikzPdfDoc;
	QString m_tikzCode;

	QProcess *m_process;
	mutable QMutex m_memberLock;
	bool m_updateScheduled;
	QWaitCondition m_updateRequested;
	QTime m_minUpdateInterval;
	QTime m_updateTimer;
	bool m_processAborted;
	bool m_runFailed;
	bool m_keepRunning;
#if QT_VERSION >= 0x040600
	QProcessEnvironment m_processEnvironment;
#else
	QStringList m_processEnvironment;
#endif

	QString m_tikzFileBaseName;
	QString m_templateFileName;
	QString m_tikzReplaceText;
//	int m_templateStartLineNumber;
	bool m_templateChanged;

	QString m_latexCommand;
	QString m_pdftopsCommand;
	QString m_shortLogText;
	QString m_logText;
	bool m_useShellEscaping;

	QProcess *m_checkGnuplotExecutable;
};

#endif
