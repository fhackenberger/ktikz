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

#ifndef KTIKZ_TIKZTEMPORARYFILECONTROLLER_H
#define KTIKZ_TIKZTEMPORARYFILECONTROLLER_H

#include <QtCore/QObject>

#ifdef KTIKZ_USE_KDE
class KTempDir;
#endif

/*!
 * \brief Creates a temporary directory and a base name for temporary files.
 *
 * This class creates a temporary directory (as a subdir of the system's
 * temp directory) and a base name for the temporary files in it.  The
 * complete base name, returned by baseName(), is guaranteed to be unique
 * for each TikZ file opened in ktikz.
 */

class TikzTemporaryFileController : public QObject
{
	Q_OBJECT

public:
	TikzTemporaryFileController(QObject *parent = 0);
	virtual ~TikzTemporaryFileController();

	const QString dirName() const;
	const QString baseName() const;
	bool cleanUp();

private:
	void createTempDir();
	void removeTempDir();

#ifdef KTIKZ_USE_KDE
	KTempDir *m_tempDir;
#endif
	QString m_tempTikzFileBaseName;
};

#endif
