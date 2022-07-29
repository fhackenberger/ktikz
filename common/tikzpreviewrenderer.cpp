/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2011, 2014 by Glad Deschrijver        *
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

#include "tikzpreviewrenderer.h"

#include <QtGui/QImage>
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
#include <QtPdf/QtPdf>
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <poppler-qt5.h>
#else
#include <poppler-qt4.h>
#endif

TikzPreviewRenderer::TikzPreviewRenderer()
{
	moveToThread(&m_thread);
	m_thread.start();
}

TikzPreviewRenderer::~TikzPreviewRenderer()
{
	if (m_thread.isRunning())
	{
		m_thread.quit();
		m_thread.wait();
	}
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
void TikzPreviewRenderer::generatePreview(QPdfDocument *tikzPdfDoc, qreal zoomFactor, QSize size, int currentPage)
{
    const QImage tikzImage = tikzPdfDoc->render(currentPage, size.scaled(zoomFactor*100, zoomFactor*100, Qt::KeepAspectRatio));

	Q_EMIT showPreview(tikzImage, zoomFactor);
}
#else
void TikzPreviewRenderer::generatePreview(Poppler::Document *tikzPdfDoc, qreal zoomFactor, int currentPage)
{
    Poppler::Page *pdfPage = tikzPdfDoc->page(currentPage);
    const QImage tikzImage = pdfPage->renderToImage(zoomFactor * 72, zoomFactor * 72);
    delete pdfPage;

    Q_EMIT showPreview(tikzImage, zoomFactor);
}
#endif
