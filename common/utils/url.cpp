/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#include "url.h"

#ifdef KTIKZ_USE_KDE
Url::Url() : KUrl()
{
}

Url::Url(const QString &fileName) : KUrl(fileName)
{
}

Url::Url(const KUrl &url) : KUrl(url)
{
}
#else
#include <QFileInfo>

Url::Url() : QUrl()
{
}

Url::Url(const QString &fileName) : QUrl(fileName)
{
}

Url::Url(const QUrl &url) : QUrl(url)
{
}

QString Url::fileName() const
{
	return QFileInfo(QUrl::path()).fileName();
}

QString Url::pathOrUrl() const
{
	return QUrl::path();
}
#endif
