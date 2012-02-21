/***************************************************************************
 *   Copyright (C) 2010, 2011, 2012 by Glad Deschrijver                    *
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

#ifndef KTIKZ_TIKZPREVIEWMESSAGEWIDGET_H
#define KTIKZ_TIKZPREVIEWMESSAGEWIDGET_H

#include <QtGui/QFrame>

class QLabel;

class TikzPreviewMessageWidget : public QFrame
{
	Q_OBJECT

public:
	enum PixmapVisibility
	{
		PixmapNotVisible = 0,
		PixmapVisible = 1
	};

	TikzPreviewMessageWidget(QWidget *parent = 0);
	~TikzPreviewMessageWidget();

	virtual QSize sizeHint() const;
	void setText(const QString &message, PixmapVisibility pixmapVisibility = PixmapNotVisible);

private:
	QSize calculateSize(bool pixmapVisible) const;

	QLabel *m_infoPixmapLabel;
	QLabel *m_infoLabel;
};

#endif
