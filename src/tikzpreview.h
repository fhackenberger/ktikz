/***************************************************************************
 *   Copyright (C) 2007-2008 by Glad Deschrijver                           *
 *   Glad.Deschrijver@UGent.be                                             *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TIKZPREVIEW_H
#define TIKZPREVIEW_H

#include <QGraphicsView>
#include <poppler-qt4.h>

class QComboBox;
class QToolBar;

class TikzPreview : public QGraphicsView
{
	Q_OBJECT

public:
	TikzPreview(QWidget *parent = 0);
	~TikzPreview();

	virtual QSize sizeHint() const;
	QList<QAction*> getActions();
	QToolBar *getViewToolBar();
	QPixmap getPixmap() const;
	void setProcessRunning(bool isRunning);

public slots:
	void pixmapUpdated(Poppler::Document *tikzPdfDoc);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
	void wheelEvent(QWheelEvent *event);

private slots:
	void setZoomFactor();
	void zoomIn();
	void zoomOut();
	void showPreviousPage();
	void showNextPage();

private:
	void centerView();
	void createActions();
	void createViewToolBar();
	void showPdfPage();
	void pixmapUpdatedEmpty();
	void pixmapUpdated();

	QGraphicsScene *m_tikzScene;
	QGraphicsPixmapItem *m_tikzPixmapItem;
	bool m_processRunning;

	QToolBar *m_viewToolBar;
	QComboBox *m_zoomCombo;
	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	QAction *m_previousPageAction;
	QAction *m_nextPageAction;

	Poppler::Document *m_tikzPdfDoc;
	int m_currentPage;
	double m_oldZoomFactor;
	double m_zoomFactor;
	double m_minZoomFactor;
	double m_maxZoomFactor;
	bool m_isZooming;
};

#endif
