/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *   Copyright (C) 2007-2008 by Glad Deschrijver                           *
 *   florian@hackenberger.at                                               *
 *   glad.deschrijver@gmail.com                                            *
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

#ifndef TIKZPREVIEWGENERATOR_H
#define TIKZPREVIEWGENERATOR_H

#include <QMutex>
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

/**
 * @author Florian Hackenberger <florian@hackenberger.at>
 */
class TikzPreviewGenerator : public QThread
{
	Q_OBJECT

public:
	TikzPreviewGenerator(const QPlainTextEdit* tikzTextEdit);
	virtual ~TikzPreviewGenerator();

	void setLatexCommand(const QString &command);
	void setPdftopsCommand(const QString &command);
	void setShellEscaping(bool useShellEscaping);
	void setReplaceText(const QString &replace);
	QString getLogText() const;
	bool hasRunFailed();
	bool exportImage(const QString &fileName, const QString &type);
	void generatePreview();

public slots:
	void setTemplateFile(const QString &fileName);
	void setTemplateFileAndRegenerate(const QString &fileName);
	void abortProcess();
	void regeneratePreview();

signals:
	void pixmapUpdated(Poppler::Document *tikzPdfDoc);
	void setExportActionsEnabled(bool enabled);
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
	bool generateEpsFile();
	bool generatePdfFile();
	bool cleanUp();

	const QPlainTextEdit *m_tikzTextEdit;
	Poppler::Document *m_tikzPdfDoc;
	bool m_tikzTextEditEmpty;

	QProcess *m_process;
	mutable QMutex m_memberLock;
	bool m_updateScheduled;
	QWaitCondition m_updateRequested;
	QTime m_minUpdateInterval;
	QTime m_updateTimer;
	bool m_processAborted;
	bool m_runFailed;
	bool m_keepRunning;

	QString m_tikzTempFileBaseName;
	QString m_templateFileName;
	QString m_tikzReplaceText;
	int m_templateStartLineNumber;
	bool m_templateChanged;

	QString m_latexCommand;
	QString m_pdftopsCommand;
	QString m_tikzString;
	QString m_shortLogText;
	QString m_logText;
	bool m_useShellEscaping;

	QProcess *m_checkGnuplotExecutable;
};

#endif
