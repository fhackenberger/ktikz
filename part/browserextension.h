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

#ifndef KTIKZ_BROWSER_EXTENSION_H
#define KTIKZ_BROWSER_EXTENSION_H

#include <KParts/BrowserExtension>

class TikzPreviewController;

namespace KtikZ
{

class BrowserExtension : public KParts::BrowserExtension
{
	Q_OBJECT
public:
	BrowserExtension(KParts::ReadOnlyPart*, TikzPreviewController *tikzPreviewController);
	~BrowserExtension();

private Q_SLOTS:
	void print();

private:
	TikzPreviewController *m_tikzPreviewController;
};

} // namespace KtikZ

#endif
