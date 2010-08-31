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

#include "tikzpreview.h"

#ifdef KTIKZ_USE_KDE
#include <KLocale>
#else
#include <QLocale>
#endif

#include <QApplication>
#include <QContextMenuEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QScrollBar>
#include <QSettings>
#include <QToolBar>

#include <poppler-qt4.h>

#include "tikzpreviewthread.h"
#include "utils/action.h"
#include "utils/icon.h"
#include "utils/selectaction.h"
#include "utils/standardaction.h"

static const qreal s_minZoomFactor = 0.1;
static const qreal s_maxZoomFactor = 6;

TikzPreview::TikzPreview(QWidget *parent)
    : QGraphicsView(parent)
{
	m_tikzScene = new QGraphicsScene(this);
	m_tikzPixmapItem = m_tikzScene->addPixmap(QPixmap());
	setScene(m_tikzScene);
	setDragMode(QGraphicsView::ScrollHandDrag);
	setWhatsThis(tr("<p>Here the preview image of "
	    "your TikZ code is shown.  You can zoom in and out, and you "
	    "can scroll the image by dragging it.</p>"));

	m_tikzPdfDoc = 0;
	m_currentPage = 0;
//	m_centerPoint = QPoint(0, 0);
	m_processRunning = false;
	m_pageSeparator = 0;

	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup("Preview");
	m_zoomFactor = settings.value("ZoomFactor", 1).toDouble();
	settings.endGroup();
	m_oldZoomFactor = -1;
	m_hasZoomed = false;

	createActions();
	createInformationLabel();
	setZoomFactor(m_zoomFactor);

	m_tikzPreviewThread = new TikzPreviewThread();
	connect(m_tikzPreviewThread, SIGNAL(showPreview(QImage)), this, SLOT(showPreview(QImage)));
}

TikzPreview::~TikzPreview()
{
	delete m_infoProxyWidget;
	delete m_tikzPreviewThread;

	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup("Preview");
	settings.setValue("ZoomFactor", m_zoomFactor);
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

	m_zoomToAction = new SelectAction(Icon("zoom-original"), tr("&Zoom"), this, "zoom_to");
	m_zoomToAction->setEditable(true);
	m_zoomToAction->setToolTip(tr("Select or insert zoom factor here"));
	m_zoomToAction->setWhatsThis(tr("<p>Select the zoom factor here.  "
	    "Alternatively, you can also introduce a zoom factor and "
	    "press Enter.</p>"));
	connect(m_zoomToAction, SIGNAL(triggered(QString)), this, SLOT(setZoomFactor(QString)));
//	createZoomFactorList();

	m_previousPageAction = new Action(Icon("go-previous"), tr("&Previous image"), this, "view_previous_image");
	m_previousPageAction->setShortcut(tr("Alt+Left", "View|Go to previous page"));
	m_previousPageAction->setStatusTip(tr("Show previous image in preview"));
	m_previousPageAction->setWhatsThis(tr("<p>Show the preview of the previous tikzpicture in the TikZ code.</p>"));
	connect(m_previousPageAction, SIGNAL(triggered()), this, SLOT(showPreviousPage()));

	m_nextPageAction = new Action(Icon("go-next"), tr("&Next image"), this, "view_next_image");
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
	viewToolBar->setObjectName("ViewToolBar");
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

void TikzPreview::createInformationLabel()
{
#ifdef KTIKZ_USE_KDE
	const QPixmap infoPixmap = KIconLoader::global()->loadIcon("dialog-error",
	    KIconLoader::Dialog, KIconLoader::SizeMedium);
#else
	const QPixmap infoPixmap = Icon("dialog-error").pixmap(QSize(32, 32));
#endif
	m_infoPixmapLabel = new QLabel;
	m_infoPixmapLabel->setPixmap(infoPixmap);

	m_infoLabel = new QLabel;

	m_infoWidget = new QFrame;
	m_infoWidget->setObjectName("infoWidget");
/*
	m_infoWidget->setStyleSheet(
	    "#infoWidget {"
	    "  background-color: palette(window);"
	    "  border: 1px solid palette(dark);"
	    "  padding: 6px;"
	    "}"
	);
*/
	m_infoWidget->setFrameShape(QFrame::Box);
	m_infoWidget->setAutoFillBackground(true);

	QPalette palette = qApp->palette();
	QColor backgroundColor = palette.window().color();
	QColor foregroundColor = palette.color(QPalette::Dark);
	backgroundColor.setAlpha(150);
	foregroundColor.setAlpha(150);
	palette.setBrush(QPalette::Window, backgroundColor);
	palette.setBrush(QPalette::WindowText, foregroundColor);
	m_infoWidget->setPalette(palette);

	palette = m_infoLabel->palette();
	foregroundColor = palette.windowText().color();
	palette.setBrush(QPalette::WindowText, foregroundColor);
	m_infoLabel->setPalette(palette);

	QHBoxLayout *infoLayout = new QHBoxLayout(m_infoWidget);
	infoLayout->setMargin(10);
	infoLayout->addWidget(m_infoPixmapLabel);
	infoLayout->addWidget(m_infoLabel);

	m_infoProxyWidget = m_tikzScene->addWidget(m_infoWidget);
	m_infoProxyWidget->setZValue(1);
//	m_infoProxyWidget->setVisible(false);
	m_tikzScene->removeItem(m_infoProxyWidget);
	m_infoWidgetAdded = false;

	m_infoPixmapLabel->setVisible(false);
}

/***************************************************************************/

void TikzPreview::centerView()
{
	m_hasZoomed = true;
}

void TikzPreview::paintEvent(QPaintEvent *event)
{
	// when m_infoWidget is visible, then it must be resized and
	// repositioned, this must be done here to do it successfully
	if (m_infoWidgetAdded && m_infoWidget->isVisible())
	{
		m_infoWidget->resize(0, 0);
		centerInfoLabel();
		m_infoWidgetAdded = false;
	}

/*
	setSceneRect(m_tikzScene->itemsBoundingRect());
	if (m_hasZoomed)
	{
		// center the viewport on the same object in the image
		// that was previously the center (in order to avoid
		// flicker, this must be done here)
//		setSceneRect(m_tikzScene->itemsBoundingRect());
		if (!m_centerPoint.isNull())
			centerOn(m_centerPoint);
		m_hasZoomed = false;
	}
*/
	if (m_hasZoomed)
	{
		// center the viewport on the same object in the image
		// that was previously the center (in order to avoid
		// flicker, this must be done here)
		const qreal zoomFraction = (m_oldZoomFactor > 0) ? m_zoomFactor / m_oldZoomFactor : 1;
		setSceneRect(m_tikzScene->itemsBoundingRect());
		centerOn((horizontalScrollBar()->value() + viewport()->width() / 2) * zoomFraction,
		    (verticalScrollBar()->value() + viewport()->height() / 2) * zoomFraction);
		m_oldZoomFactor = m_zoomFactor; // m_oldZoomFactor must be set here and not in the zoom functions below in order to avoid skipping some steps when the user zooms fast
		m_hasZoomed = false;
	}
	QGraphicsView::paintEvent(event);
}

/***************************************************************************/

QString TikzPreview::formatZoomFactor(qreal zoomFactor) const
{
#ifdef KTIKZ_USE_KDE
	QString zoomFactorText = KGlobal::locale()->formatNumber(zoomFactor, 2);
	zoomFactorText.remove(KGlobal::locale()->decimalSymbol() + "00");
	// remove trailing zero in numbers like 12.30
	if (zoomFactorText.endsWith('0')
	    && zoomFactorText.indexOf(KGlobal::locale()->decimalSymbol()) >= 0)
		zoomFactorText.chop(1);
	zoomFactorText += '%';
	return zoomFactorText;
#else
	return QLocale::system().toString(zoomFactor) + '%';
#endif
}

void TikzPreview::createZoomFactorList(qreal newZoomFactor)
{
	const qreal zoomFactorArray[] = {12.50, 25, 50, 75, 100, 125, 150, 200, 250, 300};
	const int zoomFactorNumber = 10;
	QStringList zoomFactorList;
	int newZoomFactorPosition = -1;
	bool addNewZoomFactor = true;

	if (newZoomFactor < s_minZoomFactor || newZoomFactor > s_maxZoomFactor)
		addNewZoomFactor = false;

	newZoomFactor *= 100;
	for (int i = 0; i < zoomFactorNumber; ++i)
	{
		if (addNewZoomFactor && newZoomFactor < zoomFactorArray[i])
		{
			zoomFactorList << formatZoomFactor(newZoomFactor);
			newZoomFactorPosition = i;
			addNewZoomFactor = false;
		}
		else if (newZoomFactor == zoomFactorArray[i])
		{
			newZoomFactorPosition = i;
			addNewZoomFactor = false;
		}
		zoomFactorList << formatZoomFactor(zoomFactorArray[i]);
	}
	if (addNewZoomFactor)
	{
		zoomFactorList << formatZoomFactor(newZoomFactor);
		newZoomFactorPosition = zoomFactorNumber;
	}

	disconnect(m_zoomToAction, SIGNAL(triggered(QString)), this, SLOT(setZoomFactor(QString)));
	m_zoomToAction->removeAllActions();
	m_zoomToAction->setItems(zoomFactorList);
	if (newZoomFactorPosition >= 0)
		m_zoomToAction->setCurrentItem(newZoomFactorPosition);
	connect(m_zoomToAction, SIGNAL(triggered(QString)), this, SLOT(setZoomFactor(QString)));
}

/***************************************************************************/

void TikzPreview::setZoomFactor(qreal zoomFactor)
{
//	m_oldZoomFactor = m_zoomFactor;
	m_zoomFactor = zoomFactor;
	if (m_zoomFactor == m_oldZoomFactor)
		return;

	// adjust zoom factor
	m_zoomFactor = qBound(s_minZoomFactor, m_zoomFactor, s_maxZoomFactor);

	// add current zoom factor to the list of zoom factors
	const QString zoomFactorString = formatZoomFactor(m_zoomFactor * 100);
	const int zoomFactorIndex = m_zoomToAction->items().indexOf(zoomFactorString);
	if (zoomFactorIndex >= 0)
		m_zoomToAction->setCurrentItem(zoomFactorIndex);
	else
		createZoomFactorList(m_zoomFactor);

	m_zoomInAction->setEnabled(m_zoomFactor < s_maxZoomFactor);
	m_zoomOutAction->setEnabled(m_zoomFactor > s_minZoomFactor);

	showPdfPage();
}

void TikzPreview::setZoomFactor(const QString &zoomFactorText)
{
#ifdef KTIKZ_USE_KDE
	setZoomFactor(KGlobal::locale()->readNumber(QString(zoomFactorText).remove('&').remove('%')) / 100.0);
#else
	setZoomFactor(QString(zoomFactorText).remove(QRegExp(QString("[^\\d\\%1]*").arg(QLocale::system().decimalPoint()))).toDouble() / 100);
#endif
}

void TikzPreview::zoomIn()
{
	setZoomFactor(m_zoomFactor + ((m_zoomFactor > 0.99) ?
	    (m_zoomFactor > 1.99 ? 0.5 : 0.2) : 0.1));
}

void TikzPreview::zoomOut()
{
	setZoomFactor(m_zoomFactor - ((m_zoomFactor > 1.01) ?
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

void TikzPreview::showPreview(const QImage &tikzImage)
{
	m_tikzPixmapItem->setPixmap(QPixmap::fromImage(tikzImage));
	m_tikzPixmapItem->update();
	centerView(); // adjust viewport when new objects are added to the tikz picture or when zooming
}

void TikzPreview::showPdfPage()
{
	if (!m_tikzPdfDoc || m_tikzPdfDoc->numPages() < 1)
		return;

//	m_centerPoint = QPoint(horizontalScrollBar()->value() + viewport()->width() / 2,
//	    verticalScrollBar()->value() + viewport()->height() / 2); // get current center of the viewport and use it in paintEvent

	if (!m_processRunning)
		m_tikzPreviewThread->generatePreview(m_tikzPdfDoc, m_zoomFactor, m_currentPage);
/*
	if (!m_processRunning)
	{
		Poppler::Page *pdfPage = m_tikzPdfDoc->page(m_currentPage);
		m_tikzPixmapItem->setPixmap(QPixmap::fromImage(pdfPage->renderToImage(m_zoomFactor * 72, m_zoomFactor * 72)));
		m_tikzPixmapItem->update();
		centerView(); // adjust viewport when new objects are added to the tikz picture
		delete pdfPage;
	}
*/
}

void TikzPreview::emptyPreview()
{
	m_tikzPdfDoc = 0;
	m_tikzPixmapItem->setPixmap(QPixmap());
	m_tikzPixmapItem->update();
//	m_infoWidget->setVisible(false);
	if (m_infoProxyWidget->scene() != 0) // remove error messages from view
		m_tikzScene->removeItem(m_infoProxyWidget);
	m_tikzScene->setSceneRect(0, 0, 1, 1); // remove scrollbars from view
	if (m_pageSeparator)
		m_pageSeparator->setVisible(false);
	m_previousPageAction->setVisible(false);
	m_nextPageAction->setVisible(false);
}

void TikzPreview::pixmapUpdated(Poppler::Document *tikzPdfDoc)
{
	m_tikzPdfDoc = tikzPdfDoc;

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

QPixmap TikzPreview::pixmap() const
{
	return m_tikzPixmapItem->pixmap();
}

/***************************************************************************/

void TikzPreview::centerInfoLabel()
{
	qreal posX = (sceneRect().width() - m_infoWidget->width()) / 2;
	qreal posY = (sceneRect().height() - m_infoWidget->height()) / 2;

	if (sceneRect().width() > viewport()->width())
		posX += horizontalScrollBar()->value();
	if (sceneRect().height() > viewport()->height())
		posY += verticalScrollBar()->value();

	m_infoWidget->move(posX, posY);
}

void TikzPreview::setInfoLabelText(const QString &message, bool isPixmapVisible)
{
	m_infoPixmapLabel->setVisible(isPixmapVisible);
	m_infoLabel->setText(message);
//	m_infoWidget->setVisible(false);
//	m_infoWidget->setVisible(true);
	if (m_infoProxyWidget->scene() != 0) // only remove if the widget is still attached to m_tikzScene
		m_tikzScene->removeItem(m_infoProxyWidget); // make sure that any previous messages are not visible anymore
	m_tikzScene->addItem(m_infoProxyWidget);
	m_infoWidgetAdded = true;
}

void TikzPreview::showErrorMessage(const QString &message)
{
	setInfoLabelText(message, true);
}

void TikzPreview::setProcessRunning(bool isRunning)
{
	m_processRunning = isRunning;
	if (isRunning)
		setInfoLabelText(tr("Generating image", "tikz preview status"), false);
//	else
//		m_infoWidget->setVisible(false);
	else if (m_infoProxyWidget->scene() != 0) // only remove if the widget is still attached to m_tikzScene
		m_tikzScene->removeItem(m_infoProxyWidget);
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
