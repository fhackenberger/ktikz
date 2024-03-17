/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2012 by Glad Deschrijver              *
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

#include <QtCore/QProcess>
#include <QtCore/QSettings>

QString TikzDocumentationController::tikzDocumentationPath()
{
    QSettings settings;
    QString tikzDocFile = settings.value(QLatin1String("TikzDocumentation")).toString();
    const bool tikzDocFileInSettingsEmpty = tikzDocFile.isEmpty();

    if (tikzDocFileInSettingsEmpty)
        tikzDocFile = searchTikzDocumentationInTexTree();

#ifdef KTIKZ_TIKZ_DOCUMENTATION_DEFAULT
    if (tikzDocFile.isEmpty())
        tikzDocFile = QString::fromLocal8Bit(KTIKZ_TIKZ_DOCUMENTATION_DEFAULT);
#endif

    if (tikzDocFileInSettingsEmpty && !tikzDocFile.isEmpty())
        storeTikzDocumentationPath(tikzDocFile);

    return tikzDocFile;
}

void TikzDocumentationController::storeTikzDocumentationPath(const QString &path)
{
    QSettings settings;
    settings.setValue(QLatin1String("TikzDocumentation"), path);
}

QString TikzDocumentationController::searchTikzDocumentationInTexTree()
{
    const QString kpsewhichCommand = QLatin1String("kpsewhich");
    QStringList kpsewhichArguments;
    kpsewhichArguments << QLatin1String("--format") << QLatin1String("TeX system documentation")
                       << QLatin1String("pgfmanual.pdf") << QLatin1String("pgfmanual.pdf.gz")
                       << QLatin1String("pgfmanual.ps") << QLatin1String("pgfmanual.ps.gz");

    QProcess process;
    process.start(kpsewhichCommand, kpsewhichArguments);
    process.waitForStarted(1000);
    while (process.state() != QProcess::NotRunning)
        process.waitForFinished(100 /*msec*/);

    QString tikzDocFile = QString::fromLocal8Bit(process.readAllStandardOutput().constData());
    int newLinePosition = tikzDocFile.indexOf(QLatin1Char('\n'));
    if (newLinePosition >= 0)
        tikzDocFile.remove(newLinePosition, tikzDocFile.length());
    return tikzDocFile.trimmed();
}
