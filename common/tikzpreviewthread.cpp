/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009 by Glad Deschrijver                    *
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

#include "tikzpreviewthread.h"

#include <QImage>

#include <poppler-qt4.h>

TikzPreviewThread::TikzPreviewThread(QObject *parent)
    : QThread(parent)
{
	m_restart = false;
	m_abort = false;

	setTerminationEnabled(true);
}

TikzPreviewThread::~TikzPreviewThread()
{
	m_mutex.lock();
	m_abort = true;
	m_condition.wakeOne();
	m_mutex.unlock();

	wait();
}

void TikzPreviewThread::generatePreview(Poppler::Document *tikzPdfDoc, qreal zoomFactor, int currentPage)
{
	QMutexLocker locker(&m_mutex);

	m_tikzPdfDoc = tikzPdfDoc;
	m_zoomFactor = zoomFactor;
	m_currentPage = currentPage;

	if (!isRunning())
	{
		start(LowPriority);
	}
	else
	{
//		m_restart = true;
//		m_condition.wakeOne();

		m_abort = true;
		m_condition.wakeAll();
		m_abort = false;
		m_restart = true;
//		start(LowPriority);
	}
}

void TikzPreviewThread::run()
{
	while (true)
	{
		if (m_abort)
			return;

		m_mutex.lock();
		Poppler::Page *pdfPage = m_tikzPdfDoc->page(m_currentPage);
		const qreal zoomFactor = m_zoomFactor;
		m_mutex.unlock();
		const QImage tikzImage = pdfPage->renderToImage(zoomFactor * 72, zoomFactor * 72);
		delete pdfPage;

		emit showPreview(tikzImage);

		// sleep
		m_mutex.lock();
		if (!m_restart)
			m_condition.wait(&m_mutex);
		m_restart = false;
		m_mutex.unlock();
	}
}
