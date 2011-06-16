/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009, 2011 by Glad Deschrijver              *
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

#ifndef KTIKZ_PAGEDIALOG_H
#define KTIKZ_PAGEDIALOG_H

#ifdef KTIKZ_USE_KDE
#include <KPageDialog>

class PageDialog : public KPageDialog
{
	Q_OBJECT
public:
	PageDialog(QWidget *parent = 0);
	void addPage(QWidget *widget, const QString &title, const QString &iconName);
};
#else
#include <QtGui/QDialog>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class PageDialog : public QDialog
{
	Q_OBJECT
public:
	PageDialog(QWidget *parent = 0);
	void setCaption(const QString &caption);
	void setHelp(const QString &anchor);
	void addPage(QWidget *widget, const QString &title, const QString &iconName);

private slots:
	void setCurrentPage(int page);

private:
	QWidget *centerWidget();

	QListWidget *m_pagesListWidget;
	QList<QListWidgetItem*> m_pagesListWidgetItems;
	QList<QWidget*> m_pageWidgets;
	QLabel *m_pagesTitleLabel;
	QStackedWidget *m_pagesStackedWidget;
	int m_iconWidth;
};
#endif

#endif
