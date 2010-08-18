/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#ifndef KTIKZ_SELECTACTION_H
#define KTIKZ_SELECTACTION_H

class Icon;

#ifdef KTIKZ_USE_KDE
#include <KSelectAction>

class SelectAction : public KSelectAction
{
	Q_OBJECT

public:
	explicit SelectAction(QObject *parent, const QString &name = 0);
	SelectAction(const QString &text, QObject *parent, const QString &name = 0);
	SelectAction(const Icon &icon, const QString &text, QObject *parent, const QString &name = 0);
};
#else
#include <QWidgetAction>

class QComboBox;

class SelectAction : public QWidgetAction
{
	Q_OBJECT

public:
	explicit SelectAction(QObject *parent, const QString &name = 0);
	SelectAction(const QString &text, QObject *parent, const QString &name = 0);
	SelectAction(const Icon &icon, const QString &text, QObject *parent, const QString &name = 0);
	~SelectAction();

	void setEditable(bool editable);
	void removeAllActions();
	void setItems(const QStringList &items);
	void setCurrentItem(int index);
	QStringList items() const;

signals:
	void triggered(const QString &text);

private slots:
	void setCurrentItem();

private:
	void init(const QString &name);

	QComboBox *m_selectCombo;
};
#endif

#endif
