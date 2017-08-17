/***************************************************************************
 *   Copyright (C) 2009, 2010, 2012 by Glad Deschrijver                    *
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

#ifndef KTIKZ_FILEDIALOG_H
#define KTIKZ_FILEDIALOG_H

#include "url.h"

#include <QFileDialog>


/*!
 * \brief Provides a dialog to select an URL for opening or saving.
 *
 * This class provides a dialog that allows users to select files or
 * directories.
 *
 * The purpose of this class is to provide generic API which can be used in
 * both the KDE and the Qt-only version.  Internally the KDE- or Qt-specific
 * classes and functions are used.
 *
 * Currently this class only provides the static functions getOpenUrl() and
 * getSaveUrl().
 */

class FileDialog : public QFileDialog
{
public:
	explicit FileDialog(QWidget *parent = 0, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString()) : QFileDialog(parent, caption, directory, filter) {}

	static Url getOpenUrl(QWidget *parent = 0, const QString &caption = QString(), const Url &dir = Url(), const QString &filter = QString());
	static Url getSaveUrl(QWidget *parent = 0, const QString &caption = QString(), const Url &dir = Url(), const QString &filter = QString());
};

#endif
