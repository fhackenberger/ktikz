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

#include <QtPrintSupport/QPrintPreviewWidget>
#include <QtWidgets/QVBoxLayout>

#include "action.h"
#include "globallocale.h"
#include "icon.h"
#include "standardaction.h"
#include "toolbar.h"
#include "zoomaction.h"

PrintPreviewDialog::PrintPreviewDialog(QPrinter *printer, QWidget *parent) : QDialog(parent)
{
    m_initialized = false;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_printPreviewWidget = new QPrintPreviewWidget(printer, this);
    connect(m_printPreviewWidget, &QPrintPreviewWidget::paintRequested, this,
            &PrintPreviewDialog::paintRequested);
    connect(m_printPreviewWidget, &QPrintPreviewWidget::previewChanged, this,
            &PrintPreviewDialog::updateZoomFactor);

    ToolBar *toolBar = new ToolBar(QLatin1String("printpreview_toolbar"), this);
    Action *action = new Action(Icon(QLatin1String("zoom-fit-width")), tr("Fit &width"), this,
                                QLatin1String("printpreview_fit_width"));
    connect(action, &Action::triggered, m_printPreviewWidget, &QPrintPreviewWidget::fitToWidth);
    toolBar->addAction(action);
    action = new Action(Icon(QLatin1String("zoom-fit-best")), tr("Fit p&age"), this,
                        QLatin1String("printpreview_fit_page"));
    connect(action, &Action::triggered, m_printPreviewWidget, &QPrintPreviewWidget::fitInView);
    toolBar->addAction(action);
    m_zoomToAction = new ZoomAction(Icon(QLatin1String("zoom-original")), tr("&Zoom"), this,
                                    QLatin1String("printpreview_zoom_to"));
    connect(m_zoomToAction, &ZoomAction::zoomFactorAdded, this, &PrintPreviewDialog::setZoomFactor);
    toolBar->addAction(m_zoomToAction);
    toolBar->addAction(StandardAction::zoomIn(this, SLOT(zoomIn()), this));
    toolBar->addAction(StandardAction::zoomOut(this, SLOT(zoomOut()), this));
    action = new Action(Icon(QLatin1String("document-print")), tr("&Print"), this,
                        QLatin1String("printpreview_print"));
    connect(action, &Action::triggered, this, &PrintPreviewDialog::print);
    toolBar->addAction(action);
    action = new Action(Icon(QLatin1String("window-close")), tr("&Close"), this,
                        QLatin1String("printpreview_close"));
    connect(action, &Action::triggered, this, &PrintPreviewDialog::reject);
    toolBar->addAction(action);

    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(m_printPreviewWidget);
    setLayout(mainLayout);

    m_zoomToAction->setZoomFactor(1.0);
}

void PrintPreviewDialog::setVisible(bool visible)
{
    // this will make the dialog get a decent default size
    if (visible && !m_initialized) {
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
    disconnect(m_zoomToAction, &ZoomAction::zoomFactorAdded, this,
               &PrintPreviewDialog::setZoomFactor);
    m_zoomToAction->setZoomFactor(m_printPreviewWidget->zoomFactor());
    connect(m_zoomToAction, &ZoomAction::zoomFactorAdded, this, &PrintPreviewDialog::setZoomFactor);
}

void PrintPreviewDialog::zoomIn()
{
    const qreal zoomFactor = m_printPreviewWidget->zoomFactor();
    m_zoomToAction->setZoomFactor(zoomFactor
                                  + (zoomFactor > 0.99 ? (zoomFactor > 1.99 ? 0.5 : 0.2) : 0.1));
}

void PrintPreviewDialog::zoomOut()
{
    const qreal zoomFactor = m_printPreviewWidget->zoomFactor();
    m_zoomToAction->setZoomFactor(zoomFactor
                                  - (zoomFactor > 1.01 ? (zoomFactor > 2.01 ? 0.5 : 0.2) : 0.1));
}

void PrintPreviewDialog::print()
{
    m_printPreviewWidget->print();
    accept();
}
