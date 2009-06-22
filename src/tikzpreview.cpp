/***************************************************************************
 *   Copyright (C) 2007-2008 by Glad Deschrijver                           *
 *   glad.deschrijver@gmail.com                                            *
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

#ifdef KTIKZ_USE_KDE
#include <KAction>
#include <KLocalizedString>
#include <KStandardAction>
#endif

#include <QAction>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QGraphicsPixmapItem>
#include <QLineEdit>
#include <QMenu>
#include <QScrollBar>
#include <QSettings>
#include <QToolBar>

#include <poppler-qt4.h>

#include "tikzpreview.h"

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
	m_processRunning = false;

	QSettings settings;
	m_zoomFactor = settings.value("ZoomFactor", 1).toDouble();
	m_oldZoomFactor = m_zoomFactor;
	m_minZoomFactor = 0.1;
	m_maxZoomFactor = 6;
	m_hasZoomed = false;

	createActions();
	createViewToolBar();

	setZoomFactor(m_zoomFactor);
}

TikzPreview::~TikzPreview()
{
	QSettings settings;
	settings.setValue("ZoomFactor", m_zoomFactor);
}

void TikzPreview::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu(this);
	menu->addActions(getActions());
	menu->exec(event->globalPos());
	menu->deleteLater();
}

QSize TikzPreview::sizeHint() const
{
	return QSize(250, 200);
}

void TikzPreview::centerView()
{
	m_hasZoomed = true;
}

void TikzPreview::paintEvent(QPaintEvent *event)
{
	if (m_hasZoomed)
	{
		/* center the viewport on the same object in the image
		 * that was previously the center (in order to avoid
		 * flicker, this must be done here) */
		const qreal zoomFraction = m_zoomFactor / m_oldZoomFactor;
		setSceneRect(m_tikzScene->itemsBoundingRect());
		centerOn((horizontalScrollBar()->value() + viewport()->width() / 2) * zoomFraction,
			(verticalScrollBar()->value() + viewport()->height() / 2) * zoomFraction);
		m_oldZoomFactor = m_zoomFactor; // m_oldZoomFactor must be set here and not in the zoom functions below in order to avoid skipping some steps when the user zooms fast
		m_hasZoomed = false;
	}
	QGraphicsView::paintEvent(event);
}

void TikzPreview::setZoomFactor(double zoomFactor)
{
	m_zoomFactor = zoomFactor;
	// adjust zoom factor
	if (m_zoomFactor < m_minZoomFactor)
		m_zoomFactor = m_minZoomFactor;
	else if (m_zoomFactor > m_maxZoomFactor)
		m_zoomFactor = m_maxZoomFactor;

	// add current zoom factor to the list of zoom factors
	m_zoomCombo->lineEdit()->setText(QString::number(m_zoomFactor * 100) + "%");

	m_zoomInAction->setEnabled(m_zoomFactor < m_maxZoomFactor);
	m_zoomOutAction->setEnabled(m_zoomFactor > m_minZoomFactor);

	showPdfPage();
	centerView();
}

void TikzPreview::setZoomFactor()
{
	setZoomFactor(m_zoomCombo->lineEdit()->text().remove(QRegExp("[^\\d\\.]*")).toDouble() / 100);
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

void TikzPreview::showPdfPage()
{
	if (!m_tikzPdfDoc || m_tikzPdfDoc->numPages() < 1) return;

	if (!m_processRunning)
	{
		Poppler::Page *pdfPage = m_tikzPdfDoc->page(m_currentPage);
		m_tikzPixmapItem->setPixmap(QPixmap::fromImage(pdfPage->renderToImage(m_zoomFactor * 72, m_zoomFactor * 72)));
		m_tikzPixmapItem->update();
		delete pdfPage;
	}
}

void TikzPreview::pixmapUpdated(Poppler::Document *tikzPdfDoc)
{
	m_tikzPdfDoc = tikzPdfDoc;

    if (!m_tikzPdfDoc)
	{
		m_tikzPixmapItem->setPixmap(QPixmap());
		m_tikzPixmapItem->update();
		m_previousPageAction->setVisible(false);
		m_nextPageAction->setVisible(false);
		return;
	}

	m_tikzPdfDoc->setRenderBackend(Poppler::Document::SplashBackend);
	m_tikzPdfDoc->setRenderHint(Poppler::Document::Antialiasing, true);
	m_tikzPdfDoc->setRenderHint(Poppler::Document::TextAntialiasing, true);
	const int numOfPages = m_tikzPdfDoc->numPages();

	const bool visible = (numOfPages > 1);
	m_viewToolBar->actions().at(m_viewToolBar->actions().indexOf(m_previousPageAction)-1)->setVisible(visible); // show separator
	m_previousPageAction->setVisible(visible);
	m_nextPageAction->setVisible(visible);

	if (m_currentPage >= numOfPages)
		m_currentPage = 0;

	showPdfPage();
	centerView(); // adjust viewport when new objects are added to the tikz picture
}

void TikzPreview::createActions()
{
#ifdef KTIKZ_USE_KDE
	m_zoomInAction = KStandardAction::zoomIn(this, SLOT(zoomIn()), this);
	m_zoomInAction->setWhatsThis(i18nc("@info:whatsthis", "<para>Zoom preview in by a predetermined factor</para>"));

	m_zoomOutAction = KStandardAction::zoomOut(this, SLOT(zoomOut()), this);
	m_zoomOutAction->setWhatsThis(i18nc("@info:whatsthis", "<para>Zoom preview out by a predetermined factor</para>"));

	m_previousPageAction = KStandardAction::prior(this, SLOT(showPreviousPage()), this);
	m_previousPageAction->setWhatsThis(i18nc("@info:whatsthis", "<para>Show the preview of the previous tikzpicture in the TikZ code</para>"));

	m_nextPageAction = KStandardAction::next(this, SLOT(showNextPage()), this);
	m_nextPageAction->setWhatsThis(i18nc("@info:whatsthis", "<para>Show the preview of the next tikzpicture in the TikZ code</para>"));
#else
	m_zoomInAction = new QAction(QIcon(":/images/zoom-in.png"), tr("Zoom &In"), this);
	m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
	m_zoomInAction->setStatusTip(tr("Zoom preview in"));
	m_zoomInAction->setWhatsThis(tr("<p>Zoom preview in by a predetermined factor.</p>"));
	connect(m_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

	m_zoomOutAction = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom &Out"), this);
	m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
	m_zoomOutAction->setStatusTip(tr("Zoom preview out"));
	m_zoomOutAction->setWhatsThis(tr("<p>Zoom preview out by a predetermined factor.</p>"));
	connect(m_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

	m_previousPageAction = new QAction(QIcon(":/images/go-previous.png"), tr("&Previous image"), this);
	m_previousPageAction->setShortcut(tr("Alt+Left", "View|Go to previous page"));
	m_previousPageAction->setStatusTip(tr("Show previous image in preview"));
	m_previousPageAction->setWhatsThis(tr("<p>Show the preview of the previous tikzpicture in the TikZ code.</p>"));
	connect(m_previousPageAction, SIGNAL(triggered()), this, SLOT(showPreviousPage()));

	m_nextPageAction = new QAction(QIcon(":/images/go-next.png"), tr("&Next image"), this);
	m_nextPageAction->setShortcut(tr("Alt+Right", "View|Go to next page"));
	m_nextPageAction->setStatusTip(tr("Show next image in preview"));
	m_nextPageAction->setWhatsThis(tr("<p>Show the preview of the next tikzpicture in the TikZ code.</p>"));
	connect(m_nextPageAction, SIGNAL(triggered()), this, SLOT(showNextPage()));
#endif

	m_previousPageAction->setVisible(false);
	m_previousPageAction->setEnabled(false);
	m_nextPageAction->setVisible(false);
	m_nextPageAction->setEnabled(true);
}

QList<QAction*> TikzPreview::getActions()
{
	QList<QAction*> actions;
	actions << m_zoomInAction << m_zoomOutAction;
	QAction *action = new QAction(this);
	action->setSeparator(true);
	actions << action;
	actions << m_previousPageAction << m_nextPageAction;
	return actions;
}

void TikzPreview::createViewToolBar()
{
	QStringList zoomSizesList;
	zoomSizesList << "12.50%" << "25%" << "50%" << "75%" << "100%" << "125%" << "150%" << "200%" << "250%" << "300%";
	m_zoomCombo = new QComboBox;
	m_zoomCombo->setEditable(true);
	m_zoomCombo->setToolTip(tr("Select or insert zoom factor here"));
	m_zoomCombo->setWhatsThis(tr("<p>Select the zoom factor here.  "
	    "Alternatively, you can also introduce a zoom factor and "
	    "press Enter.</p>"));
	m_zoomCombo->insertItems(0, zoomSizesList);
	const QString zoomText = QString::number(m_zoomFactor * 100) + "%";
	m_zoomCombo->setCurrentIndex(m_zoomCombo->findText(zoomText));
	m_zoomCombo->lineEdit()->setText(zoomText);
	connect(m_zoomCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setZoomFactor()));
	connect(m_zoomCombo->lineEdit(), SIGNAL(returnPressed()), this, SLOT(setZoomFactor()));

	m_viewToolBar = new QToolBar(tr("View"));
	m_viewToolBar->setObjectName("ViewToolBar");
	m_viewToolBar->addAction(m_zoomInAction);
	m_viewToolBar->addWidget(m_zoomCombo);
	m_viewToolBar->addAction(m_zoomOutAction);
	m_viewToolBar->addSeparator();
	m_viewToolBar->addAction(m_previousPageAction);
	m_viewToolBar->addAction(m_nextPageAction);
	m_viewToolBar->actions().at(m_viewToolBar->actions().indexOf(m_previousPageAction)-1)->setVisible(false);
}

QToolBar *TikzPreview::getViewToolBar()
{
	return m_viewToolBar;
}

QPixmap TikzPreview::getPixmap() const
{
	return m_tikzPixmapItem->pixmap();
}

void TikzPreview::setProcessRunning(bool isRunning)
{
	m_processRunning = isRunning;
}

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
