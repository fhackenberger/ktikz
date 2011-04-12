/***************************************************************************
 *   Copyright (C) 2011 by Glad Deschrijver                                *
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

#include "browserextension.h"

#include <KParts/Part>
#include <KIconLoader>

#include "../common/tikzpreviewcontroller.h"

BrowserExtension::BrowserExtension(KParts::ReadOnlyPart *part, TikzPreviewController *tikzPreviewController)
    : KParts::BrowserExtension(part)
{
	m_part = part;
	m_tikzPreviewController = tikzPreviewController;
	emit enableAction("print", true);
	QString iconPath = KIconLoader::global()->iconPath("image-x-generic", KIconLoader::SizeSmall);
	emit setIconUrl(KUrl::fromPath(iconPath));
}

BrowserExtension::~BrowserExtension()
{
}

void BrowserExtension::print()
{
	m_tikzPreviewController->printImage();
}
