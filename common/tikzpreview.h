/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2014                *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
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

#ifndef KTIKZ_TIKZPREVIEW_H
#define KTIKZ_TIKZPREVIEW_H

#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QGraphicsView>
#else
#include <QtGui/QGraphicsView>
#endif
#include "tikzpreviewmessagewidget.h"

class QToolBar;

namespace Poppler
{
class Document;
}

class Action;
class ZoomAction;
class TikzPreviewMessageWidget;
class TikzPreviewRenderer;

class TikzPreview : public QGraphicsView
{
	Q_OBJECT

public:
	explicit TikzPreview(QWidget *parent = 0);
	~TikzPreview();

	virtual QSize sizeHint() const;
	QList<QAction*> actions();
	QToolBar *toolBar();
	QImage renderToImage(double xres, double yres, int pageNumber);
	QPixmap pixmap() const;
	int currentPage() const;
	int numberOfPages() const;
	void emptyPreview();
	void setProcessRunning(bool isRunning);
	void setShowCoordinates(bool show);
	void setCoordinatePrecision(int precision);
	void setBackgroundColor(QColor color);

public Q_SLOTS:
	void showPreview(const QImage &tikzImage, qreal zoomFactor = 1.0);
	void pixmapUpdated(Poppler::Document *tikzPdfDoc, const QList<qreal> &tikzCoordinates = QList<qreal>());
	void showErrorMessage(const QString &message);

Q_SIGNALS:
	void showMouseCoordinates(qreal x, qreal y, int precisionX = 5, int precisionY = 5);
	void generatePreview(Poppler::Document *tikzPdfDoc, qreal zoomFactor, int currentPage);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);

private Q_SLOTS:
	void setZoomFactor(qreal zoomFactor);
	void zoomIn();
	void zoomOut();
	void showPreviousPage();
	void showNextPage();

private:
	void createInformationLabel();
	void createActions();
	void showPdfPage();
	void centerInfoLabel();
	void setInfoLabelText(const QString &message, TikzPreviewMessageWidget::PixmapVisibility pixmapVisibility = TikzPreviewMessageWidget::PixmapNotVisible);

	QGraphicsScene *m_tikzScene;
	QGraphicsPixmapItem *m_tikzPixmapItem;
	TikzPreviewRenderer *m_tikzPreviewRenderer;
	bool m_processRunning;

	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	ZoomAction *m_zoomToAction;
	QAction *m_pageSeparator;
	Action *m_previousPageAction;
	Action *m_nextPageAction;

	TikzPreviewMessageWidget *m_infoWidget;

	Poppler::Document *m_tikzPdfDoc;
	int m_currentPage;
	qreal m_zoomFactor;
	qreal m_oldZoomFactor;
	bool m_hasZoomed;

	bool m_showCoordinates;
	QList<qreal> m_tikzCoordinates;
	int m_precision;
};

#endif
