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

#ifndef KTIKZ_PRINTPREVIEWDIALOG_H
#define KTIKZ_PRINTPREVIEWDIALOG_H

#include <QtGui/QDialog>

class QPrinter;
class QPrintPreviewWidget;
class ZoomAction;

class PrintPreviewDialog : public QDialog
{
	Q_OBJECT
public:
	explicit PrintPreviewDialog(QPrinter *printer, QWidget *parent = 0);

	void setVisible(bool visible);

signals:
	void paintRequested(QPrinter *printer);

private slots:
	void setZoomFactor(qreal zoomFactor);
	void updateZoomFactor();
	void zoomIn();
	void zoomOut();
	void print();

private:
	QPrintPreviewWidget *m_printPreviewWidget;
	ZoomAction *m_zoomToAction;
	bool m_initialized;
};

#endif
