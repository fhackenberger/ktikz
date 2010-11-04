/***************************************************************************
 *   Copyright (C) 2010 by Glad Deschrijver                                *
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

#include "file.h"

#ifdef KTIKZ_USE_KDE
//#include <KDebug>
#include <KIO/Job>
//#include <KIO/JobUiDelegate>
#include <KIO/NetAccess>

QWidget *File::s_mainWidget;
QString File::s_tempDir;

File::File(const QString &fileName, const OpenMode &mode)
{
	m_openMode = mode;
	m_url = Url(fileName);

	load();
}

File::File(const Url &url, const OpenMode &mode)
{
	m_openMode = mode;
	m_url = url;

	load();
}

/*!
 * This function is used internally by the constructors to load the file
 * for reading or writing.  In the KDE version, if the file is remote, the
 * file will be copied to the temporary directory which is previously set
 * by setTempDir().
 */

void File::load()
{
	m_errorString = QString();

	if (!m_url.isValid())
	{
		m_file = new QFile();
		return;
	}

	// We assume for simplicity that s_tempDir is not empty, this is the
	// case because a TikzPreviewController object is created at startup
	// and in its constructor setTempDir() is called.
	Q_ASSERT_X(!s_tempDir.isEmpty(), "loading a file", "File::setTempDir(const QString &dirName) must be called before constructing any File object");
	m_localFileName = m_url.isLocalFile() ? m_url.path() : s_tempDir + m_url.fileName();

	if (m_openMode == WriteOnly)
	{
		m_file = new KSaveFile(m_localFileName);
	}
	else if (m_openMode == ReadOnly)
	{
		if (!m_url.isLocalFile() && KIO::NetAccess::exists(m_url, KIO::NetAccess::SourceSide, s_mainWidget))
		{
			KIO::Job *job = KIO::file_copy(m_url, KUrl::fromPath(m_localFileName), -1, KIO::Overwrite | KIO::HideProgressInfo);
			if (!KIO::NetAccess::synchronousRun(job, s_mainWidget))
			{
				m_errorString = tr("Could not copy \"%1\" to temporary file \"%2\".").arg(m_url.prettyUrl()).arg(m_localFileName);
				return;
			}
		}
		m_file = new QFile(m_localFileName);
	}
}

File::~File()
{
	close();
	delete m_file;
}

bool File::open(const QFile::OpenMode &mode)
{
	if (m_openMode == WriteOnly)
	{
		m_errorString = QString();
		return dynamic_cast<KSaveFile*>(m_file)->open(); // XXX cannot use qobject_cast because KSaveFile doesn't have the Q_OBJECT macro
	}
	else if (m_openMode == ReadOnly)
	{
		if (!m_errorString.isEmpty()) // when the file is not a local file and copying the file to a local file using KIO failed
			return false;
		return m_file->open(QFile::ReadOnly | mode);
	}
	return false;
}

bool File::close()
{
	m_errorString = QString();

	if (m_openMode == WriteOnly)
	{
		if (!dynamic_cast<KSaveFile*>(m_file)->finalize()) // XXX cannot use qobject_cast because KSaveFile doesn't have the Q_OBJECT macro
			return false;
	}
	m_file->close();

	if (m_openMode == WriteOnly && !m_url.isLocalFile())
	{
		KIO::Job *job = KIO::file_copy(KUrl::fromPath(m_localFileName), m_url, -1, KIO::Overwrite | KIO::HideProgressInfo);
		if (!KIO::NetAccess::synchronousRun(job, s_mainWidget))
		{
			m_errorString = tr("Could not copy temporary file \"%1\" to \"%2\".").arg(m_localFileName).arg(m_url.prettyUrl());
			return false;
		}
	}
	return true;
}

/*
 * This slot displays KIO job errors (if any).
 * \param job the KIO job the errors of which are displayed
 */

/*
void File::showJobError(KJob *job)
{
	if (job->error() != 0)
	{
		KIO::JobUiDelegate *ui = static_cast<KIO::Job*>(job)->ui();
		if (!ui)
		{
			kError() << "Saving failed; job->ui() is null.";
			return;
		}
		ui->setWindow(s_mainWidget);
		ui->showErrorMessage();
	}
}
*/

bool File::copy(const Url &fromUrl, const Url &toUrl)
{
	KIO::Job *job = KIO::file_copy(fromUrl, toUrl, -1, KIO::Overwrite | KIO::HideProgressInfo);
	return KIO::NetAccess::synchronousRun(job, s_mainWidget);
}

/*!
 * This function sets the main widget which will be used as a parent for all
 * KIO dialogs used for copying remote files in the KDE version.  This function
 * \b must be called prior to creating any File object.
 * \param widget a QWidget which will be the parent widget for the KIO dialogs, typically this is the main window
 */

void File::setMainWidget(QWidget *widget)
{
	s_mainWidget = widget;
}

/*!
 * This function sets the temporary directory in which all remote files will
 * be copied by KIO in order to use them locally.  This function
 * \b must be called prior to creating any File object.
 * \param dirName the name of the temporary directory in which the local versions of the remote files are stored
 */

void File::setTempDir(const QString &dirName)
{
	s_tempDir = dirName;
}
#else
//#include <QCoreApplication>
//#include <QMessageBox>
#include <QFileInfo>

/*!
 * Constructs a File object.
 * \param fileName the name of the file to be opened
 * \param mode the mode in which the file will be opened
 */

File::File(const QString &fileName, const OpenMode &mode)
{
	m_openMode = mode;
	m_file = new QFile(fileName);
}

/*!
 * Constructs a File object.
 * \param url the url of the file to be opened
 * \param mode the mode in which the file will be opened
 */

File::File(const Url &url, const OpenMode &mode)
{
	m_openMode = mode;
	m_file = new QFile(url.path());
}

File::~File()
{
	close();
	delete m_file;
}

/*!
 * This function opens the file.
 * \param mode additional modes in which a device is opened (you should
 * \b not use QFile::ReadOnly or QFile::WriteOnly at this point!)
 * \return true if successful, false otherwise
 */

bool File::open(const QFile::OpenMode &mode)
{
	if (m_openMode == WriteOnly)
		return m_file->open(QFile::WriteOnly | mode);
	else if (m_openMode == ReadOnly)
		return m_file->open(QFile::ReadOnly | mode);
	return false;
}

/*!
 * This function closes the file.
 * \return true if successful, false otherwise
 */

bool File::close()
{
	m_file->close();
	return true;
}

/*!
 * This function copies the file specified by \em fromUrl to \em toUrl.
 * An error message is displayed when the copying has failed.
 * \param fromUrl the URL of the file to be copied
 * \param toUrl the URL to which the file is copied
 * \return true if successful, false otherwise
 */

bool File::copy(const Url &fromUrl, const Url &toUrl)
{
	const QString toFileName = toUrl.path();
	if (QFileInfo(toFileName).exists() && !QFile::remove(toFileName))
	{
//		QMessageBox::critical(s_mainWidget, QCoreApplication::applicationName(),
//		    tr("The file \"%1\" could not be overwritten").arg(toFileName));
		return false;
	}
//	if (!QFile::copy(fromUrl.path(), toUrl.path()))
//		QMessageBox::critical(s_mainWidget, QCoreApplication::applicationName(),
//		    tr("The file \"%1\" could not be copied to").arg(toFileName));
	return QFile::copy(fromUrl.path(), toFileName);
}
#endif

/*!
 * This function returns a pointer to the QFile object created by this class.
 * If in normal Qt code you would pass a QFile as argument to some function,
 * you must now pass file() instead.
 *
 * Example:
 * instead of using
 * \code
 *     QFile f("filename.txt");
 *     if (!f.open(QFile::ReadOnly | QFile::Text))
 *         return;
 *     QTextStream ts(&f);
 * \endcode
 * the following is used:
 * \code
 *     File f("filename.txt", File::ReadOnly);
 *     if (!f.open(QFile::Text))
 *         return;
 *     QTextStream ts(f.file());
 * \endcode
 *
 * \return the QFile object embedded by this class
 */

QFile *File::file()
{
	return m_file;
}

/*!
 * Returns a human-readable description of the last file IO error that occurred.
 * \return a string describing the last file IO error
 */

QString File::errorString() const
{
	if (!m_errorString.isEmpty())
		return m_errorString;
	return m_file->errorString();
}
