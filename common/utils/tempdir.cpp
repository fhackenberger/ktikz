/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2014                            *
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

#include "tempdir.h"

#include <QtCore/QDir>

// #include <KStandardDirs>
#include <QTemporaryDir>

TempDir::TempDir(const QString &directoryPrefix) : QTemporaryDir(directoryPrefix)
{
    setAutoRemove(true);
}

const QString TempDir::location() const
{
    // 	const QString tempDirBase = KStandardDirs::locateLocal("tmp",
    // KGlobal::mainComponent().componentName()); 	const int end =
    // tempDirBase.lastIndexOf(QLatin1Char('/')); 	return tempDirBase.mid(0, end);
    return QTemporaryDir::path();
}

/*!
 * Removes all files in the temporary directory.
 */

bool TempDir::cleanUp()
{
    const QString dirName = path();
    if (dirName.isEmpty()) // we must return in this case, otherwise in the QDir constructor below,
                           // the program's working directory is used and we really don't want to
                           // remove the files in that
        return false;

    QDir tempDir(dirName);
    if (!tempDir.exists())
        return false;

    bool success = true;
    const QStringList fileList = tempDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QString &fileName: fileList) {
        success = success && tempDir.remove(fileName);
    }

    return success;
}
