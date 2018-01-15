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

#include "filedialog.h"

#include <QCoreApplication>

/*!
 * Parses a KDE-like filter and returns a Qt-like filter.  The filter
 * "All files (*)" is automatically appended in order to cover the case
 * in which Qt's own file dialog (in which the filter entry cannot be
 * cleared similarly as in KDE's file dialog) is visible.
 * \param filter a filter in KDE style
 * \return a filter in Qt style
 */

static QString getParsedFilter(const QString &filter)
{
	QString parsedFilter;
	if (filter.indexOf(QLatin1Char('/')) >= 0) // filter is a string like "text/x-pgf text/x-tex"
	{
		const QStringList mimeTypeList = filter.split(QLatin1Char(' '));
		for (int i = 0; i < mimeTypeList.size(); ++i)
		{
			if (i > 0)
				parsedFilter += QLatin1String(";;");
			if (mimeTypeList.at(i) == QLatin1String("text/x-pgf"))
				parsedFilter += QCoreApplication::translate("FileDialog", "PGF document", "filter") + QLatin1String(" (*.pgf *.tikz)");
			else if (mimeTypeList.at(i) == QLatin1String("image/x-eps"))
				parsedFilter += QCoreApplication::translate("FileDialog", "EPS image", "filter") + QLatin1String(" (*.eps)");
			else if (mimeTypeList.at(i) == QLatin1String("application/pdf"))
				parsedFilter += QCoreApplication::translate("FileDialog", "PDF document", "filter") + QLatin1String(" (*.pdf)");
			else if (mimeTypeList.at(i).startsWith(QLatin1String("image/")))
			{
				const QString mimeType = mimeTypeList.at(i).mid(6);
				parsedFilter += QCoreApplication::translate("FileDialog", "%1 image", "filter").arg(mimeType.toUpper()) + QLatin1String(" (*.") + mimeType + QLatin1Char(')');
			}
		}
	}
	else if (filter.indexOf(QLatin1Char('|')) >= 0) // filter is a string like "*.pgf *.tikz|PGF document\n*.tex|TeX document", we assume that in this case '/' doesn't appear in filter
	{
		const QStringList filterList = filter.split(QLatin1Char('\n'));
		for (int i = 0; i < filterList.size(); ++i)
		{
			const QStringList filterItems = filterList.at(i).split(QLatin1Char('|'));
			if (i > 0)
				parsedFilter += QLatin1String(";;");
			parsedFilter += filterItems.at(1) + QLatin1String(" (") + filterItems.at(0) + QLatin1Char(')');
		}
	}
	if (!parsedFilter.isEmpty())
		parsedFilter += QLatin1String(";;");
	parsedFilter += QCoreApplication::translate("FileDialog", "All files", "filter") + QLatin1String(" (*)");
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
 *         QString("*.pgf *.tikz"|%1\n*.tex|%2").arg(tr("PGF files")).arg(tr("TeX files")));
 * \endcode
 *
 * or, alternatively,
 *
 * \code
 *     Url url = FileDialog::getOpenUrl(this,
 *         tr("Open PGF Source File"),
 *         Url("/home/user/filename.pgf"),
 *         "text/x-pgf text/x-tex");
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
 * \param filter a list of filters separated by '\\n'; every filter entry is defined through \c namefilter|text \c to \c display.  If no '|' is found in the expression, just the namefilter is shown.  In the Qt-only version the filter "All files (*)" is automatically appended to cover the case in which the "Filter" entry of the dialog cannot be cleared (as in KDE's file dialog).  Alternatively, a list of mimetypes separated by a space can be given.  For better consistency across applications in a KDE environment, it is recommended to use the latter option.
 * \return an url specifying the file selected by the user in the file dialog
 */

Url FileDialog::getOpenUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	const QUrl openFileName = QFileDialog::getOpenFileUrl(parent, caption, dir, getParsedFilter(filter));
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
 *         tr("Save PGF Source File"),
 *         Url("/home/user/filename.pgf"),
 *         QString("*.pgf *.tikz"|%1\n*.tex|%2").arg(tr("PGF files")).arg(tr("TeX files")));
 * \endcode
 *
 * or, alternatively,
 *
 * \code
 *     Url url = FileDialog::getSaveUrl(this,
 *         tr("Save PGF Source File"),
 *         Url("/home/user/filename.pgf"),
 *         "text/x-pgf text/x-tex");
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
 * \param filter a list of filters separated by '\\n'; every filter entry is defined through \c namefilter|text \c to \c display.  If no '|' is found in the expression, just the namefilter is shown.  In the Qt-only version the filter "All files (*)" is automatically appended to cover the case in which the "Filter" entry of the dialog cannot be cleared (as in KDE's file dialog).  Alternatively, a list of mimetypes separated by a space can be given.  For better consistency across applications in a KDE environment, it is recommended to use the latter option.
 * \return an url specifying the file selected by the user in the file dialog
 */

Url FileDialog::getSaveUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	const QUrl saveAsFileName = QFileDialog::getSaveFileUrl(parent, caption, dir, getParsedFilter(filter));
	if (saveAsFileName.isEmpty())
		return Url();

	return Url(saveAsFileName);
}
