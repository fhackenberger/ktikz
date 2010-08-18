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

#include "tikzdocumentationcontroller.h"

#include <QProcess>
#include <QSettings>

QString TikzDocumentationController::tikzDocumentationPath()
{
	QSettings settings(ORGNAME, APPNAME);
	QString tikzDocFile = settings.value("TikzDocumentation").toString();
	const bool tikzDocFileInSettingsEmpty = tikzDocFile.isEmpty();

	if (tikzDocFileInSettingsEmpty)
		tikzDocFile = searchTikzDocumentationInTexTree();

#ifdef KTIKZ_TIKZ_DOCUMENTATION_DEFAULT
	if (tikzDocFile.isEmpty())
		tikzDocFile = KTIKZ_TIKZ_DOCUMENTATION_DEFAULT;
#endif

	if (tikzDocFileInSettingsEmpty && !tikzDocFile.isEmpty())
		storeTikzDocumentationPath(tikzDocFile);

	return tikzDocFile;
}

void TikzDocumentationController::storeTikzDocumentationPath(const QString &path)
{
	QSettings settings(ORGNAME, APPNAME);
	settings.setValue("TikzDocumentation", path);
}

QString TikzDocumentationController::searchTikzDocumentationInTexTree()
{
	const QString kpsewhichCommand = "kpsewhich";
	QStringList kpsewhichArguments;
	kpsewhichArguments << "--format" << "TeX system documentation" << "pgfmanual.pdf" << "pgfmanual.pdf.gz" << "pgfmanual.ps" << "pgfmanual.ps.gz";

	QProcess process;
	process.start(kpsewhichCommand, kpsewhichArguments);
	process.waitForStarted(1000);
	while (process.state() != QProcess::NotRunning)
		process.waitForFinished(100 /*msec*/);

	QString tikzDocFile = process.readAllStandardOutput();
	return tikzDocFile.trimmed();
}
