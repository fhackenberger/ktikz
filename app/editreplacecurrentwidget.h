/***************************************************************************
 *   Copyright (C) 2008, 2011, 2014 by Glad Deschrijver                    *
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

#ifndef REPLACECURRENTWIDGET_H
#define REPLACECURRENTWIDGET_H

#include <QtCore/QtGlobal>
#include <QtWidgets/QWidget>

class QLabel;
class QPushButton;

class ReplaceCurrentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReplaceCurrentWidget(QWidget *parent = 0);
    ~ReplaceCurrentWidget();
    void setReplacement(const QString &text, const QString &replacement);

Q_SIGNALS:
    void search();
    void replace();
    void replaceAll();
    void hidden();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

protected Q_SLOTS:
    void hide();

private Q_SLOTS:
    void dontReplace();

private:
    QLabel *m_replaceLabel;
    QPushButton *m_replaceButton;
};

#endif
