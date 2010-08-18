/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2010 by Glad Deschrijver              *
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

#ifndef KTIKZ_TIKZPREVIEW_H
#define KTIKZ_TIKZPREVIEW_H

#include <QGraphicsView>

class QComboBox;
class QImage;
class QLabel;
class QToolBar;

namespace Poppler
{
	class Document;
}

class Action;
class SelectAction;
class TikzPreviewThread;

class TikzPreview : public QGraphicsView
{
	Q_OBJECT

public:
	TikzPreview(QWidget *parent = 0);
	~TikzPreview();

	virtual QSize sizeHint() const;
	QList<QAction*> actions();
	QToolBar *toolBar();
	QPixmap pixmap() const;
	void emptyPreview();
	void setProcessRunning(bool isRunning);

public slots:
	void showPreview(const QImage &tikzImage);
	void pixmapUpdated(Poppler::Document *tikzPdfDoc);
	void showErrorMessage(const QString &message);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
	void wheelEvent(QWheelEvent *event);

private slots:
	void setZoomFactor(const QString &zoomFactorText);
	void zoomIn();
	void zoomOut();
	void showPreviousPage();
	void showNextPage();

private:
	void createInformationLabel();
	void centerView();
	void setZoomFactor(qreal zoomFactor);
	void createActions();
	void showPdfPage();
	void centerInfoLabel();
	void setInfoLabelText(const QString &message, bool isPixmapVisible);
	QString formatZoomFactor(qreal zoomFactor) const;
	void createZoomFactorList(qreal newZoomFactor = 0);

	QGraphicsScene *m_tikzScene;
	QGraphicsPixmapItem *m_tikzPixmapItem;
	TikzPreviewThread *m_tikzPreviewThread;
//	QPoint m_centerPoint;
	bool m_processRunning;

	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	SelectAction *m_zoomToAction;
	QAction *m_pageSeparator;
	Action *m_previousPageAction;
	Action *m_nextPageAction;

	QFrame *m_infoWidget;
	QGraphicsItem *m_infoProxyWidget;
	QLabel *m_infoPixmapLabel;
	QLabel *m_infoLabel;
	bool m_infoWidgetAdded;

	Poppler::Document *m_tikzPdfDoc;
	int m_currentPage;
	qreal m_zoomFactor;
	qreal m_oldZoomFactor;
	bool m_hasZoomed;
};

#endif
