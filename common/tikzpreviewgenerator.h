/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2014                *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
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

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QThread>

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
 * @author Glad Deschrijver <glad.deschrijver@gmail.com>
 */
class TikzPreviewGenerator : public QObject
{
	Q_OBJECT

public:
	enum TemplateStatus
	{
		DontReloadTemplate = 0,
		ReloadTemplate = 1
	};

	explicit TikzPreviewGenerator(TikzPreviewController *parent);
	~TikzPreviewGenerator();

	void setTikzFileBaseName(const QString &name);
	void setLatexCommand(const QString &command);
	void setPdftopsCommand(const QString &command);
	void setShellEscaping(bool useShellEscaping);
	QString getLogText() const;
	bool hasRunFailed();
	void addToLatexSearchPath(const QString &path);
	void removeFromLatexSearchPath(const QString &path);
	bool generateEpsFile(int page);

public Q_SLOTS:
	void setTemplateFile(const QString &fileName);
	void setReplaceText(const QString &replace);
	void generatePreview(TemplateStatus templateStatus = DontReloadTemplate);
	void abortProcess();

Q_SIGNALS:
	void pixmapUpdated(Poppler::Document *tikzPdfDoc, const QList<qreal> &tikzCoordinates = QList<qreal>());
	void setExportActionsEnabled(bool enabled);
	void showErrorMessage(const QString &message);
	void updateLog(const QString &logText, bool runFailed);
	void appendLog(const QString &logText, bool runFailed);
	void processRunning(bool isRunning);

private Q_SLOTS:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	void displayGnuplotNotExecutable();
	void checkGnuplotExecutableFinished(int exitCode, QProcess::ExitStatus exitStatus);
#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	void generatePreviewImpl(TemplateStatus templateStatus = DontReloadTemplate);

protected:
	void parseLogFile();
	void createPreview();
	void showFileWriteError(const QString &fileName, const QString &errorMessage);
	bool runProcess(const QString &name, const QString &command, const QStringList &arguments, const QString &workingDir = QString());
	bool generatePdfFile(const QString &tikzFileBaseName, const QString &latexCommand, bool useShellEscaping);

	TikzPreviewController *m_parent;
	Poppler::Document *m_tikzPdfDoc;
	QString m_tikzCode;

	QThread m_thread;

	QProcess *m_process;
	mutable QMutex m_memberLock;
	bool m_processAborted;
	bool m_runFailed;
	QProcessEnvironment m_processEnvironment;
	bool m_firstRun;

	QString m_tikzFileBaseName;
	QString m_templateFileName;
	QString m_tikzReplaceText;
	bool m_templateChanged;

	QString m_latexCommand;
	QString m_pdftopsCommand;
	QString m_shortLogText;
	QString m_logText;
	bool m_useShellEscaping;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QProcess *m_checkGnuplotExecutable;
#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
};

#endif
