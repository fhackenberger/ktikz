/***************************************************************************
 *   Copyright (C) 2009, 2010 by Glad Deschrijver                          *
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

#include "filedialog.h"

#ifdef KTIKZ_USE_KDE
Url FileDialog::getOpenUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	return KFileDialog::getOpenUrl(dir, filter, parent, caption);
}

Url FileDialog::getSaveUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	return KFileDialog::getSaveUrl(dir, filter, parent, caption, KFileDialog::ConfirmOverwrite);
}
#else
#include <QtCore/QCoreApplication>

/*!
 * Parses a KDE-like filter and returns a Qt-like filter
 * \param filter a filter in KDE style
 * \return a filter in Qt style
 */

QString FileDialog::getParsedFilter(const QString &filter)
{
	const QStringList filterList = filter.split(QLatin1Char('\n'));
	QString parsedFilter;
	for (int i = 0; i < filterList.size(); ++i)
	{
		const QStringList filterItems = filterList.at(i).split(QLatin1Char('|'));
		if (i > 0)
			parsedFilter += ";;";
		parsedFilter += filterItems.at(1) + " (" + filterItems.at(0) + ')';
	}
	return parsedFilter;
}

/*!
 * Creates a modal file dialog and returns the selected URL or an empty
 * string if none was chosen.  With this method the user must select an
 * existing URL.
 *
 * \code
 *     Url url = FileDialog::getOpenUrl(this,
 *         tr("Open PGF Source File"),
 *         Url("/home/user/filename.pgf"),
 *         QString("*.pgf *.tikz"|%1\n*|%2").arg(tr("PGF files")).arg(tr("All files")));
 * \endcode
 *
 * The function creates a modal dialog with the given \em parent widget.
 * If \em parent is not 0, the dialog will be shown centered over the parent
 * widget.
 *
 * The file dialog's working directory will be set to \em dir.  If \em dir
 * includes a file name, the file will be selected.  Only files that match
 * a given \em filter are shown.
 * \param parent the parent widget for which the file dialog will be a modal dialog
 * \param caption the title of the file dialog
 * \param dir starting directory; if dir includes a file name, the file will be selected
 * \param filter a list of filters separated by '\\n'; every filter entry is defined through \c namefilter|text \c to \c display.  If no '|' is found in the expression, just the namefilter is shown.
 * \return an url specifying the file selected by the user in the file dialog
 */

Url FileDialog::getOpenUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	const QString openFileName = QFileDialog::getOpenFileName(parent, caption, dir.path(), getParsedFilter(filter));
	if (openFileName.isEmpty())
		return Url();
	return Url(openFileName);
}

/*!
 * Creates a modal file dialog and returns the selected URL or an empty
 * string if none was chosen.  With this method the user need not select
 * an existing URL.
 *
 * \code
 *     Url url = FileDialog::getSaveUrl(this,
 *         tr("Open PGF Source File"),
 *         Url("/home/user/filename.pgf"),
 *         QString("*.pgf *.tikz"|%1\n*|%2").arg(tr("PGF files")).arg(tr("All files")));
 * \endcode
 *
 * The function creates a modal dialog with the given \em parent widget.
 * If \em parent is not 0, the dialog will be shown centered over the parent
 * widget.
 *
 * The file dialog's working directory will be set to \em dir.  If \em dir
 * includes a file name, the file will be selected.  Only files that match
 * a given \em filter are shown.
 * \param parent the parent widget for which the file dialog will be a modal dialog
 * \param caption the title of the file dialog
 * \param dir starting directory; if dir includes a file name, the file will be selected
 * \param filter a list of filters separated by '\\n'; every filter entry is defined through \c namefilter|text \c to \c display.  If no '|' is found in the expression, just the namefilter is shown.
 * \return an url specifying the file selected by the user in the file dialog
 */

Url FileDialog::getSaveUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	const QString saveAsFileName = QFileDialog::getSaveFileName(parent, caption, dir.path(), getParsedFilter(filter));
	if (saveAsFileName.isEmpty())
		return Url();

	return Url(saveAsFileName);
}
#endif
