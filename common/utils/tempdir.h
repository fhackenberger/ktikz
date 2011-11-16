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

#ifndef KTIKZ_TEMPDIR_H
#define KTIKZ_TEMPDIR_H

#ifdef KTIKZ_USE_KDE
#include <KTempDir>

class TempDir : public KTempDir
{
public:
	TempDir(const QString &directoryPrefix = QString(), int mode = 0700);

	const QString location() const;
	bool cleanUp();
};
#else
#include <QtCore/QObject>

/*!
 * \brief Creates a temporary directory with a unique name.
 *
 * This class creates a temporary directory (as a subdir of the system's
 * temp directory).  The directory name, returned by name(), is guaranteed
 * to be unique.
 */

class TempDir : public QObject
{
public:
	TempDir(const QString &directoryPrefix = QString(), int mode = 0700);
	virtual ~TempDir();

	const QString location() const;
	const QString name() const;
	bool cleanUp();

private:
	QString m_name;
};
#endif

#endif
