/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2011 by Glad Deschrijver              *
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

#ifndef KTIKZ_TIKZPREVIEWRENDERER_H
#define KTIKZ_TIKZPREVIEWRENDERER_H

#include <QtCore/QThread>

class QImage;

namespace Poppler
{
class Document;
}

class TikzPreviewRenderer : public QObject
{
	Q_OBJECT

public:
	TikzPreviewRenderer();
	~TikzPreviewRenderer();

public slots:
	void generatePreview(Poppler::Document *tikzPdfDoc, qreal zoomFactor = 1.0, int currentPage = 0);

signals:
	void showPreview(const QImage &image, qreal zoomFactor = 1.0);

private:
	QThread m_thread;
};

#endif
