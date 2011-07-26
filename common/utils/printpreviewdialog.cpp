/***************************************************************************
 *   Copyright (C) 2011 by Glad Deschrijver                                *
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

#include "printpreviewdialog.h"

#include <QtGui/QPrintPreviewWidget>
#include <QtGui/QVBoxLayout>

#include "action.h"
#include "globallocale.h"
#include "icon.h"
#include "standardaction.h"
#include "toolbar.h"
#include "zoomaction.h"

PrintPreviewDialog::PrintPreviewDialog(QPrinter *printer, QWidget *parent)
	: QDialog(parent)
{
	m_initialized = false;

	QVBoxLayout *mainLayout = new QVBoxLayout;

	m_printPreviewWidget = new QPrintPreviewWidget(printer, this);
	connect(m_printPreviewWidget, SIGNAL(paintRequested(QPrinter*)), this, SIGNAL(paintRequested(QPrinter*)));
	connect(m_printPreviewWidget, SIGNAL(previewChanged()), this, SLOT(updateZoomFactor()));

	ToolBar *toolBar = new ToolBar("printpreview_toolbar", this);
	Action *action = new Action(Icon("zoom-fit-width"), tr("Fit &width"), this, "printpreview_fit_width");
	connect(action, SIGNAL(triggered()), m_printPreviewWidget, SLOT(fitToWidth()));
	toolBar->addAction(action);
	action = new Action(Icon("zoom-fit-best"), tr("Fit p&age"), this, "printpreview_fit_page");
	connect(action, SIGNAL(triggered()), m_printPreviewWidget, SLOT(fitInView()));
	toolBar->addAction(action);
	m_zoomToAction = new ZoomAction(Icon("zoom-original"), tr("&Zoom"), this, "printpreview_zoom_to");
	connect(m_zoomToAction, SIGNAL(zoomFactorAdded(qreal)), this, SLOT(setZoomFactor(qreal)));
	toolBar->addAction(m_zoomToAction);
	toolBar->addAction(StandardAction::zoomIn(this, SLOT(zoomIn()), this));
	toolBar->addAction(StandardAction::zoomOut(this, SLOT(zoomOut()), this));
	action = new Action(Icon("document-print"), tr("&Print"), this, "printpreview_print");
	connect(action, SIGNAL(triggered()), this, SLOT(print()));
	toolBar->addAction(action);
	action = new Action(Icon("window-close"), tr("&Close"), this, "printpreview_close");
	connect(action, SIGNAL(triggered()), this, SLOT(reject()));
	toolBar->addAction(action);

	mainLayout->addWidget(toolBar);
	mainLayout->addWidget(m_printPreviewWidget);
	setLayout(mainLayout);

	m_zoomToAction->setZoomFactor(1.0);
}

void PrintPreviewDialog::setVisible(bool visible)
{
	// this will make the dialog get a decent default size
	if (visible && !m_initialized)
	{
		m_printPreviewWidget->updatePreview();
		m_initialized = true;
	}
	QDialog::setVisible(visible);
}

void PrintPreviewDialog::setZoomFactor(qreal zoomFactor)
{
	m_printPreviewWidget->setZoomFactor(zoomFactor);
}

void PrintPreviewDialog::updateZoomFactor()
{
	disconnect(m_zoomToAction, SIGNAL(zoomFactorAdded(qreal)), this, SLOT(setZoomFactor(qreal)));
	m_zoomToAction->setZoomFactor(m_printPreviewWidget->zoomFactor());
	connect(m_zoomToAction, SIGNAL(zoomFactorAdded(qreal)), this, SLOT(setZoomFactor(qreal)));
}

void PrintPreviewDialog::zoomIn()
{
	const qreal zoomFactor = m_printPreviewWidget->zoomFactor();
	m_zoomToAction->setZoomFactor(zoomFactor + (zoomFactor > 0.99 ?
	                                           (zoomFactor > 1.99 ? 0.5 : 0.2) : 0.1));
}

void PrintPreviewDialog::zoomOut()
{
	const qreal zoomFactor = m_printPreviewWidget->zoomFactor();
	m_zoomToAction->setZoomFactor(zoomFactor - (zoomFactor > 1.01 ?
	                                           (zoomFactor > 2.01 ? 0.5 : 0.2) : 0.1));
}

void PrintPreviewDialog::print()
{
	m_printPreviewWidget->print();
	accept();
}
