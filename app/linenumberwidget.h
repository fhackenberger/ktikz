/***************************************************************************
 *   Copyright (C) 2007, 2008, 2011, 2012 by Glad Deschrijver              *
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

#ifndef LINENUMBERWIDGET_H
#define LINENUMBERWIDGET_H

#include <QtGui/QPen>
#include <QtWidgets/QWidget>

class TikzEditor;

class LineNumberWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberWidget(TikzEditor *editor);
    QSize sizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *) override;

private:
    TikzEditor *m_editor;
    QColor m_highlightColor;
    QBrush m_highlightBrush;
    QPen m_highlightPen;
    QPen m_highlightedTextPen;
};

#endif // LINENUMBERWIDGET_H
