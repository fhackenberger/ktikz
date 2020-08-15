/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014          *
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

#include "tikzpreview.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <poppler-qt5.h>
#else
#include <poppler-qt4.h>
#endif

#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QScrollBar>
#include <QToolBar>

#include "app/configeditorwidget.h"
#include "tikzpreviewrenderer.h"
#include "utils/action.h"
#include "utils/icon.h"
#include "utils/standardaction.h"
#include "utils/zoomaction.h"

TikzPreview::TikzPreview(QWidget *parent)
	: QGraphicsView(parent)
	, m_processRunning(false)
	, m_pageSeparator(0)
	, m_infoWidget(0)
	, m_tikzPdfDoc(0)
	, m_currentPage(0)
	, m_oldZoomFactor(-1)
	, m_hasZoomed(false)
{
	m_tikzScene = new QGraphicsScene(this);
	m_tikzPixmapItem = m_tikzScene->addPixmap(QPixmap());
	setScene(m_tikzScene);
	setDragMode(QGraphicsView::ScrollHandDrag);
	m_tikzPixmapItem->setCursor(Qt::CrossCursor);
	setWhatsThis(tr("<p>Here the preview image of "
	                "your TikZ code is shown.  You can zoom in and out, and you "
	                "can scroll the image by dragging it.</p>"));

	QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
	settings.beginGroup(QLatin1String("Preview"));
	m_zoomFactor = settings.value(QLatin1String("ZoomFactor"), 1).toDouble();
	setBackgroundColor(settings.value(QLatin1String("PreviewBackgroundColor"), ConfigEditorWidget::defaultSetting(QLatin1String("PreviewBackgroundColor"))).value<QColor>());
	settings.endGroup();

	createActions();

	m_tikzPreviewRenderer = new TikzPreviewRenderer();
	connect(this, SIGNAL(generatePreview(Poppler::Document*,qreal,int)), m_tikzPreviewRenderer, SLOT(generatePreview(Poppler::Document*,qreal,int)));
	connect(m_tikzPreviewRenderer, SIGNAL(showPreview(QImage,qreal)), this, SLOT(showPreview(QImage,qreal)));
}

TikzPreview::~TikzPreview()
{
	delete m_tikzPixmapItem;
	delete m_infoWidget;
	delete m_tikzPreviewRenderer;

	QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
	settings.beginGroup(QLatin1String("Preview"));
	settings.setValue(QLatin1String("ZoomFactor"), m_zoomFactor);
	settings.endGroup();
}

void TikzPreview::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu(this);
	menu->addActions(actions());
	menu->exec(event->globalPos());
	menu->deleteLater();
}

QSize TikzPreview::sizeHint() const
{
	const int screenWidth = QApplication::desktop()->availableGeometry().width();
	if (screenWidth > 1200)
		return QSize(500, 400);
	else if (screenWidth > 1024)
		return QSize(400, 400);
	else
		return QSize(250, 200);
}

/***************************************************************************/

void TikzPreview::createActions()
{
	m_zoomInAction = StandardAction::zoomIn(this, SLOT(zoomIn()), this);
	m_zoomOutAction = StandardAction::zoomOut(this, SLOT(zoomOut()), this);
	m_zoomInAction->setStatusTip(tr("Zoom preview in"));
	m_zoomOutAction->setStatusTip(tr("Zoom preview out"));
	m_zoomInAction->setWhatsThis(tr("<p>Zoom preview in by a predetermined factor.</p>"));
	m_zoomOutAction->setWhatsThis(tr("<p>Zoom preview out by a predetermined factor.</p>"));

	m_zoomToAction = new ZoomAction(Icon(QLatin1String("zoom-original")), tr("&Zoom"), this, QLatin1String("zoom_to"));
	m_zoomToAction->setZoomFactor(m_zoomFactor);
	connect(m_zoomToAction, SIGNAL(zoomFactorAdded(qreal)), this, SLOT(setZoomFactor(qreal)));

	m_previousPageAction = new Action(Icon(QLatin1String("go-previous")), tr("&Previous image"), this, QLatin1String("view_previous_image"));
	m_previousPageAction->setShortcut(tr("Alt+Left", "View|Go to previous page"));
	m_previousPageAction->setStatusTip(tr("Show previous image in preview"));
	m_previousPageAction->setWhatsThis(tr("<p>Show the preview of the previous tikzpicture in the TikZ code.</p>"));
	connect(m_previousPageAction, SIGNAL(triggered()), this, SLOT(showPreviousPage()));

	m_nextPageAction = new Action(Icon(QLatin1String("go-next")), tr("&Next image"), this, QLatin1String("view_next_image"));
	m_nextPageAction->setShortcut(tr("Alt+Right", "View|Go to next page"));
	m_nextPageAction->setStatusTip(tr("Show next image in preview"));
	m_nextPageAction->setWhatsThis(tr("<p>Show the preview of the next tikzpicture in the TikZ code.</p>"));
	connect(m_nextPageAction, SIGNAL(triggered()), this, SLOT(showNextPage()));

	m_previousPageAction->setVisible(false);
	m_previousPageAction->setEnabled(false);
	m_nextPageAction->setVisible(false);
	m_nextPageAction->setEnabled(true);
}

QList<QAction*> TikzPreview::actions()
{
	QList<QAction*> actions;
	actions << m_zoomInAction << m_zoomOutAction;
	QAction *action = new QAction(this);
	action->setSeparator(true);
	actions << action;
	actions << m_previousPageAction << m_nextPageAction;
	return actions;
}

QToolBar *TikzPreview::toolBar()
{
	QToolBar *viewToolBar = new QToolBar(tr("View"), this);
	viewToolBar->setObjectName(QLatin1String("ViewToolBar"));
	viewToolBar->addAction(m_zoomInAction);
	viewToolBar->addAction(m_zoomToAction);
	viewToolBar->addAction(m_zoomOutAction);
	m_pageSeparator = viewToolBar->addSeparator();
	m_pageSeparator->setVisible(false);
	viewToolBar->addAction(m_previousPageAction);
	viewToolBar->addAction(m_nextPageAction);
	return viewToolBar;
}

/***************************************************************************/

void TikzPreview::paintEvent(QPaintEvent *event)
{
	if (m_hasZoomed)
	{
		setSceneRect(m_tikzScene->itemsBoundingRect()); // make sure that the scroll area is not bigger than the actual image
		m_hasZoomed = false;
	}
	if (m_infoWidget && m_infoWidget->isVisible()) // make sure that if the error that Gnuplot is not found is shown at startup, then it is shown in the center of the view; this does not happen automatically because the error is generated before the main window becomes visible
		centerInfoLabel();

	QGraphicsView::paintEvent(event);
}

/***************************************************************************/

void TikzPreview::setZoomFactor(qreal zoomFactor)
{
	m_zoomFactor = zoomFactor;
	if (m_zoomFactor == m_oldZoomFactor)
		return;

	m_zoomInAction->setEnabled(m_zoomFactor < m_zoomToAction->maxZoomFactor());
	m_zoomOutAction->setEnabled(m_zoomFactor > m_zoomToAction->minZoomFactor());

	showPdfPage();
}

void TikzPreview::zoomIn()
{
	m_zoomToAction->setZoomFactor(m_zoomFactor + (m_zoomFactor > 0.99 ?
	                                             (m_zoomFactor > 1.99 ? 0.5 : 0.2) : 0.1));
}

void TikzPreview::zoomOut()
{
	m_zoomToAction->setZoomFactor(m_zoomFactor - (m_zoomFactor > 1.01 ?
	                                             (m_zoomFactor > 2.01 ? 0.5 : 0.2) : 0.1));
}

/***************************************************************************/

void TikzPreview::showPreviousPage()
{
	if (m_currentPage > 0)
		--m_currentPage;
	m_previousPageAction->setEnabled(m_currentPage > 0);
	m_nextPageAction->setEnabled(m_currentPage < m_tikzPdfDoc->numPages() - 1);
	showPdfPage();
}

void TikzPreview::showNextPage()
{
	if (m_currentPage < m_tikzPdfDoc->numPages() - 1)
		++m_currentPage;
	m_previousPageAction->setEnabled(m_currentPage > 0);
	m_nextPageAction->setEnabled(m_currentPage < m_tikzPdfDoc->numPages() - 1);
	showPdfPage();
}

void TikzPreview::showPreview(const QImage &tikzImage, qreal zoomFactor)
{
	// this slot is called when TikzPreviewRenderer has finished rendering
	// the current pdf page to tikzImage, so before we actually display
	// the image the old center point must be calculated and multiplied
	// by the quotient of the new and old zoom factor in order to obtain
	// the new center point of the image; the recentering itself is done
	// at the end of this function
	QPointF centerPoint(horizontalScrollBar()->value() + viewport()->width() * 0.5,
	                    verticalScrollBar()->value() + viewport()->height() * 0.5);
	const qreal zoomFraction = (m_oldZoomFactor > 0) ? zoomFactor / m_oldZoomFactor : 1;
	if (!centerPoint.isNull())
		centerPoint *= zoomFraction;
	m_oldZoomFactor = zoomFactor; // m_oldZoomFactor must be set here and not in the zoom functions in order to avoid skipping some steps when the user zooms fast
	m_hasZoomed = true;

	// display and center the preview image
	m_tikzPixmapItem->setPixmap(QPixmap::fromImage(tikzImage));
	centerOn(centerPoint);
}

void TikzPreview::showPdfPage()
{
	if (!m_tikzPdfDoc || m_tikzPdfDoc->numPages() < 1)
		return;

	if (!m_processRunning)
		Q_EMIT generatePreview(m_tikzPdfDoc, m_zoomFactor, m_currentPage); // render the current pdf page to a QImage in TikzPreviewRenderer (in a different thread)
}

void TikzPreview::emptyPreview()
{
	m_tikzPdfDoc = 0;
	m_tikzCoordinates.clear();
	m_tikzPixmapItem->setPixmap(QPixmap());
	m_tikzPixmapItem->update();
	if (m_infoWidget)
		m_infoWidget->setVisible(false); // remove error messages from view
	setSceneRect(m_tikzScene->itemsBoundingRect()); // remove scrollbars from view
	if (m_pageSeparator)
		m_pageSeparator->setVisible(false);
	m_previousPageAction->setVisible(false);
	m_nextPageAction->setVisible(false);
}

void TikzPreview::pixmapUpdated(Poppler::Document *tikzPdfDoc, const QList<qreal> &tikzCoordinates)
{
	m_tikzPdfDoc = tikzPdfDoc;
	m_tikzCoordinates = tikzCoordinates;

	if (!m_tikzPdfDoc)
	{
		emptyPreview();
		return;
	}

	m_tikzPdfDoc->setRenderBackend(Poppler::Document::SplashBackend);
//	m_tikzPdfDoc->setRenderBackend(Poppler::Document::ArthurBackend);
	m_tikzPdfDoc->setRenderHint(Poppler::Document::Antialiasing, true);
	m_tikzPdfDoc->setRenderHint(Poppler::Document::TextAntialiasing, true);
	const int numOfPages = m_tikzPdfDoc->numPages();

	const bool visible = (numOfPages > 1);
	if (m_pageSeparator)
		m_pageSeparator->setVisible(visible);
	m_previousPageAction->setVisible(visible);
	m_nextPageAction->setVisible(visible);

	if (m_currentPage >= numOfPages) // if the new tikz code has fewer tikzpictures than the previous one (this may happen if a new PGF file is opened in the same window), then we must reset m_currentPage
	{
		m_currentPage = 0;
		m_previousPageAction->setEnabled(false);
		m_nextPageAction->setEnabled(true);
	}

	showPdfPage();
}

/***************************************************************************/

QImage TikzPreview::renderToImage(double xres, double yres, int pageNumber)
{
	Poppler::Page *page = m_tikzPdfDoc->page(pageNumber);
//	const QSizeF pageSize = page->pageSizeF();
//	const QImage image = pageSize.height() >= pageSize.width()
//		? page->renderToImage(xres, yres)
//		: page->renderToImage(xres, yres, -1, -1, -1, -1, Poppler::Page::Rotate270); // slow
	const QImage image = page->renderToImage(xres, yres); // slow
	delete page;
	return image;
}

QPixmap TikzPreview::pixmap() const
{
	return m_tikzPixmapItem->pixmap();
}

int TikzPreview::currentPage() const
{
	return m_currentPage;
}

int TikzPreview::numberOfPages() const
{
	return m_tikzPdfDoc->numPages();
}

/***************************************************************************/

void TikzPreview::createInformationLabel()
{
	m_infoWidget = new TikzPreviewMessageWidget(this);
	QGraphicsItem *infoProxyWidget = m_tikzScene->addWidget(m_infoWidget);
	infoProxyWidget->setZValue(1);
	m_infoWidget->setVisible(false);
}

void TikzPreview::centerInfoLabel()
{
	const qreal posX = (viewport()->width() - m_infoWidget->width()) * 0.5;
	const qreal posY = (viewport()->height() - m_infoWidget->height()) * 0.5;
	m_infoWidget->move(posX, posY);
}

void TikzPreview::setInfoLabelText(const QString &message, TikzPreviewMessageWidget::PixmapVisibility pixmapVisibility)
{
	if (!m_infoWidget)
		createInformationLabel();
	m_infoWidget->setText(message, pixmapVisibility);
	m_infoWidget->setVisible(true);
	centerInfoLabel(); // must be run here so that the label is always centered
}

void TikzPreview::showErrorMessage(const QString &message)
{
	setInfoLabelText(message, TikzPreviewMessageWidget::PixmapVisible);
}

void TikzPreview::setProcessRunning(bool isRunning)
{
	m_processRunning = isRunning;
	if (isRunning)
		setInfoLabelText(tr("Generating image", "tikz preview status"), TikzPreviewMessageWidget::PixmapNotVisible);
	else
		m_infoWidget->setVisible(false);
}

/***************************************************************************/

void TikzPreview::setShowCoordinates(bool show)
{
	m_showCoordinates = show;
}

void TikzPreview::setCoordinatePrecision(int precision)
{
	m_precision = precision;
}

void TikzPreview::setBackgroundColor(QColor color)
{
	m_tikzScene->setBackgroundBrush(color);
}

/***************************************************************************/

void TikzPreview::wheelEvent(QWheelEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier)
	{
		if (event->delta() > 0)
			zoomIn();
		else
			zoomOut();
	}
	else
		QGraphicsView::wheelEvent(event);
}

void TikzPreview::mouseMoveEvent(QMouseEvent *event)
{
	const int offset = 6 * m_currentPage;
	if (m_showCoordinates && m_tikzCoordinates.length() >= offset + 6)
	{
		const qreal unitX = m_tikzCoordinates.at(offset); // unit length in x-direction in points
		const qreal unitY = m_tikzCoordinates.at(1 + offset); // unit length in y-direction in points
		const qreal minX = m_tikzCoordinates.at(2 + offset); // minimum x-coordinate on the figure in points
		const qreal maxX = m_tikzCoordinates.at(3 + offset); // maximum x-coordinate on the figure in points
		const qreal minY = m_tikzCoordinates.at(4 + offset); // minimum y-coordinate on the figure in points
		const qreal maxY = m_tikzCoordinates.at(5 + offset); // maximum y-coordinate on the figure in points

		if (unitX > 0 && unitY > 0) // this is not the case for 3D plots
		{
			int precisionX = m_precision; // the number of decimals used to display the x-coordinate of the mouse pointer
			int precisionY = m_precision; // idem for the y-coordinate
			if (m_precision < 0) // in app/configgeneralwidget.cpp the precision is set to -1 if the user chooses "Best precision", which we calculate now
			{
				qreal invUnitX = 1 / unitX;
				qreal invUnitY = 1 / unitY;
				for (precisionX = 0; invUnitX < 1; ++precisionX) // make sure that some significant decimals are displayed (and not numbers like 0.00)
					invUnitX *= 10;
				for (precisionY = 0; invUnitY < 1; ++precisionY) // idem
					invUnitY *= 10;
			}

			const QPointF mouseSceneCoords = mapToScene(event->pos()) / m_zoomFactor;
			const qreal coordX = mouseSceneCoords.x() + minX;
			const qreal coordY = maxY - mouseSceneCoords.y();
			if (coordX >= minX && coordX <= maxX && coordY >= minY && coordY <= maxY)
				Q_EMIT showMouseCoordinates(coordX / unitX, coordY / unitY, precisionX, precisionY);
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void TikzPreview::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::XButton1)
		showPreviousPage();
	else if (event->button() == Qt::XButton2)
		showNextPage();
	QGraphicsView::mousePressEvent(event);
}
