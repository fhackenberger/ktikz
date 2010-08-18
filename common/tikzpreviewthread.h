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

#ifndef KTIKZ_TIKZPREVIEWTHREAD_H
#define KTIKZ_TIKZPREVIEWTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QImage;

namespace Poppler
{
	class Document;
}

class TikzPreviewThread : public QThread
{
	Q_OBJECT

public:
	TikzPreviewThread(QObject *parent = 0);
	~TikzPreviewThread();

	void generatePreview(Poppler::Document *tikzPdfDoc, qreal zoomFactor = 1.0, int currentPage = 0);

signals:
	void showPreview(const QImage &image);

protected:
	void run();

private:
	QMutex m_mutex;
	QWaitCondition m_condition;
	bool m_restart;
	bool m_abort;

	Poppler::Document *m_tikzPdfDoc;
	qreal m_zoomFactor;
	int m_currentPage;
};

#endif
