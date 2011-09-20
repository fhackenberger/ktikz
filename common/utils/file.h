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

#ifndef KTIKZ_FILE_H
#define KTIKZ_FILE_H

#include <QtCore/QObject>
#include <QtCore/QFile>

#ifdef KTIKZ_USE_KDE
#include <KSaveFile>
class KJob;
#endif

#include "url.h"

class Url;

/*!
 * \brief Wrapper around a QFile or, in the KDE version, KSaveFile object.
 *
 * This class is a wrapper around a QFile object.  In the KDE version, if the
 * OpenMode is File::WriteOnly, then it is a wrapper around a KSaveFile object.
 * This class also handles opening and saving remote files using KIO
 * transparently.
 *
 * \section Initialization
 * Before any File object is created, the main widget to which the dialogs
 * created by File must be modal and the temporary directory in which File
 * will put temporary files must be specified:
 * \code
 *     File::setMainWidget(mainWindow);
 *     File::setTempDir("/path/to/existing/directory");
 * \endcode
 * This must be done only once during the execution of the application,
 * before any File object is created.
 *
 * \section File-reading-files Reading files
 * The following example shows how to read a text file:
 * \code
 *     File f("filename.txt", File::ReadOnly);
 *     if (!f.open(QFile::Text))
 *         qDebug() << "Cannot read file filename.txt: " + f.errorString();
 *     QTextStream ts(f.file());
 *     QString fileContents = ts.readAll();
 *     if (!f.close())
 *         qDebug() << "Cannot close file filename.txt: " + f.errorString();
 * \endcode
 * When using KDE the first argument to the File constructor can also be a
 * remote URL (the rest of the code doesn't change in this case).
 *
 * The file doesn't have to be closed explicitly.  It is closed when the File
 * object is destroyed.
 *
 * \section File-writing-files Writing files:
 * The next example shows how to write a file:
 * \code
 *     File f("filename.txt", File::WriteOnly);
 *     if (!f.open())
 *         qDebug() << "Cannot write file filename.txt: " + f.errorString();
 *     QTextStream ts(f.file());
 *     ts << "Some text.\n";
 *     if (!f.close())
 *         qDebug() << "Cannot close file filename.txt: " + f.errorString();
 * \endcode
 * When using KDE the first argument to the File constructor can also be a
 * remote URL (the rest of the code doesn't change in this case).
 *
 * The file doesn't have to be closed explicitly.  It is closed when the File
 * object is destroyed.
 */

class File : public QObject
{
	Q_OBJECT

public:
	/*!
	 * This enum is used with the constructor to describe the mode in which
	 * the file will be opened.
	 */
	enum OpenMode
	{
		ReadOnly, /*!<The file will be opened for reading. */
		WriteOnly /*!<The file will be opened for writing. */
	};

	File(const QString &fileName, const OpenMode &mode);
	File(const Url &url, const OpenMode &mode);
	~File();

	bool open(const QFile::OpenMode &mode = 0);
	bool close();
	QFile *file();
	QString errorString() const;

	static bool copy(const Url &fromUrl, const Url &toUrl);

#ifdef KTIKZ_USE_KDE
	static void setMainWidget(QWidget *widget);
	static void setTempDir(const QString &dirName);

//private slots:
//	void showJobError(KJob *job);
#endif

private:
	void load();

	OpenMode m_openMode;
	QFile *m_file;

#ifdef KTIKZ_USE_KDE
	static QWidget *s_mainWidget;
	static QString s_tempDir;
#endif

	Url m_url;
	QString m_localFileName;

	QString m_errorString;
};

#endif
