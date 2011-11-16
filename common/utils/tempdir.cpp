/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011 by Glad Deschrijver              *
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

#include "tempdir.h"

#include <QtCore/QDir>

#ifdef KTIKZ_USE_KDE
#include <KComponentData>
#include <KStandardDirs>
#include <KTempDir>

TempDir::TempDir(const QString &directoryPrefix, int mode)
	: KTempDir(directoryPrefix, mode)
{
	setAutoRemove(true);
}

const QString TempDir::location() const
{
	const QString tempDirBase = KStandardDirs::locateLocal("tmp", KGlobal::mainComponent().componentName());
	const int end = tempDirBase.lastIndexOf(QLatin1Char('/'));
	return tempDirBase.mid(0, end);
}
#else
#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryFile>

/*!
 * Creates a temporary directory with the name:
 *  \p \<directoryPrefix\>\<six letters\>
 *
 * In the KDE version the default \p directoryPrefix is "$KDEHOME/tmp-$HOST/appname", i.e.
 * as returned by KStandardDirs::locateLocal("tmp", KGlobal::mainComponent().componentName())
 * In the Qt-only version the default is QDir::tempPath() + "/qtikz"
 *
 * @param directoryPrefix the prefix of the file name, or
 *        QString() for the default value
 * @param mode the file permissions,
 * almost always in octal. The first digit selects permissions for
 * the user who owns the file: read (4), write (2), and execute
 * (1); the second selects permissions for other users in the
 * file's group, with the same values; and the third for other
 * users not in the file's group, with the same values.
 */

TempDir::TempDir(const QString &directoryPrefix, int mode)
{
	Q_UNUSED(mode);

	// use QTemporaryFile to obtain a unique name
	const QString dirPrefix = (directoryPrefix.isEmpty() ? QDir::tempPath() + "/qtikz" : directoryPrefix);
	QTemporaryFile *tempFile = new QTemporaryFile(dirPrefix + "XXXXXX");
	if (tempFile->open())
	{
		const QFileInfo tempFileInfo = QFileInfo(*tempFile);
		m_name = tempFileInfo.absolutePath() + '/' + tempFileInfo.completeBaseName();
	}
	else
		qCritical("Error: could not create temporary directory");
	delete tempFile; // this also deletes the file on disk, so the creation of a directory with the same name below can be done successfully

	// we have a unique name, let's create the directory
	QDir tempDir(m_name);
	const QString dirName = tempDir.dirName();
	if (!tempDir.cdUp() || !tempDir.mkdir(dirName)) // by construction, QDir::temp() is the parent of tempDir
		m_name.clear();
}

TempDir::~TempDir()
{
	// removing all temporary files
	qDebug("removing temporary files");

	if (!cleanUp())
	{
		qCritical("Error: removing temporary files failed");
		return;
	}

	// remove temp dir if empty
	QDir tempDir(m_name);
	if (tempDir.exists())
	{
		const QString dirName = tempDir.dirName();
		if (!tempDir.cdUp() || !tempDir.rmdir(dirName))
			qCritical("Error: removing temporary directory failed");
	}
}

/*!
 * This function returns the path of the directory in which the temporary
 * directory of this program is (attempted to be) created.  Usually this is
 * the system's temporary directory.  If the creation of the temporary
 * directory of this application fails, the function name() will return an
 * empty QString(), so this function can be used to inform
 * the user about the location in which the temporary directory should have
 * been created.
 * \return the name of the system's temporary directory
 */

const QString TempDir::location() const
{
	return QDir::tempPath();
}

/*!
 * This function returns the full path and name of the temporary directory.
 * \return the name of the temporary directory, or QString() if creating the directory has failed
 */

const QString TempDir::name() const
{
	return m_name;
}
#endif

/*!
 * Removes all files in the temporary directory.
 */

bool TempDir::cleanUp()
{
	const QString dirName = name();
	if (dirName.isEmpty()) // we must return in this case, otherwise in the QDir constructor below, the program's working directory is used and we really don't want to remove the files in that
		return false;

	QDir tempDir(dirName);
	if (!tempDir.exists())
		return false;

	bool success = true;
	const QStringList fileList = tempDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
	foreach (const QString &fileName, fileList)
		success = success && tempDir.remove(fileName);
	return success;
}
