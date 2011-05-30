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

#include "tikztemporaryfilecontroller.h"

#ifdef KTIKZ_USE_KDE
#include <KTempDir>
#else
#include <QtCore/QTemporaryFile>
#endif

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

TikzTemporaryFileController::TikzTemporaryFileController(QObject *parent)
	: QObject(parent)
{
	createTempDir();
}

TikzTemporaryFileController::~TikzTemporaryFileController()
{
	removeTempDir();
}

/***************************************************************************/

#ifdef KTIKZ_USE_KDE
void TikzTemporaryFileController::createTempDir()
{
	m_tempDir = new KTempDir();
	m_tempDir->setAutoRemove(true);
	m_tempTikzFileBaseName = m_tempDir->name() + "temptikzcode";
}

void TikzTemporaryFileController::removeTempDir()
{
	delete m_tempDir;
}

const QString TikzTemporaryFileController::dirName() const
{
	return m_tempDir->name();
}
#else
/*!
 * Creates the temporary directory and the base name for the temporary files
 * used for the current tikz image.
 */

void TikzTemporaryFileController::createTempDir()
{
	QDir tempDir(QDir::tempPath() + "/qtikz");
	if (!tempDir.exists())
		QDir::temp().mkdir("qtikz");
	QTemporaryFile *tempFile = new QTemporaryFile(QDir::tempPath() + "/qtikz/temptikzcodeXXXXXX.tex");
	if (tempFile->open())
	{
		const QFileInfo tempFileInfo = QFileInfo(*tempFile);
		m_tempTikzFileBaseName = tempFileInfo.absolutePath() + '/' + tempFileInfo.completeBaseName();
	}
	else
		qCritical() << "Error: could not create temporary file in" << QDir::toNativeSeparators(QDir::tempPath() + "/qtikz/");
	delete tempFile;
}

/*!
 * Removes all temporary files for the current tikz image and then removes
 * the temporary directory if it is empty.  Normally, the temporary directory
 * is non-empty only after a crash of ktikz.
 */

void TikzTemporaryFileController::removeTempDir()
{
	// removing all temporary files
	qDebug("removing tempfiles");
	if (!m_tempTikzFileBaseName.isEmpty() && !cleanUp())
		qCritical("Error: removing tempfiles failed");

	// remove temp dir if empty
	QDir tempDir(QDir::tempPath() + "/qtikz");
	if (tempDir.exists())
		QDir::temp().rmdir("qtikz");
}

/*!
 * This function return the name of the temporary directory in which the
 * temporary files needed to compile the tikz image are located.
 * \return the name of the temporary directory
 */

const QString TikzTemporaryFileController::dirName() const
{
	return QDir::tempPath() + "/qtikz";
}
#endif

/*!
 * This function returns the complete base name of the temporary files
 * with the path.  The complete base name consists of all characters in
 * the file name up to (but not including) the last '.' character.
 * \return the complete base name of the temporary files with the path
 */

const QString TikzTemporaryFileController::baseName() const
{
	return m_tempTikzFileBaseName;
}

/*!
 * Removes all auxiliary files needed in the compilation of the current
 * tikz file.  This function removes all files for which the complete
 * base name coincides with the result of baseName().
 */

bool TikzTemporaryFileController::cleanUp()
{
#ifndef KTIKZ_USE_KDE
	// test whether the temporary directory still exists and recreate if it
	// doesn't exist
	// this is necessary because if an empty window is opened and another
	// window is opened and closed, then the temporary directory is deleted
	// and no file can be compiled in the first window, no static count
	// variable can be declared which stores the number of open windows,
	// because when windows are closed in another process and the temporary
	// directory is removed, we cannot know this in the current process
	// we do the test here because in tikzpreviewcontroller.cpp this function
	// is always called before generating a preview
	QDir tempDir(QDir::tempPath() + "/qtikz");
	if (!tempDir.exists())
		QDir::temp().mkdir("qtikz");
#endif

	bool success = true;

	const QFileInfo tempTikzFileInfo(m_tempTikzFileBaseName + ".tex");
	QDir tempTikzDir(tempTikzFileInfo.absolutePath());
	const QStringList fileList = tempTikzDir.entryList(QStringList()
	                             << tempTikzFileInfo.completeBaseName() + ".*");

	foreach (const QString &fileName, fileList)
		success = success && tempTikzDir.remove(fileName);

	return success;
}
