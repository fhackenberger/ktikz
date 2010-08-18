/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
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

#include <QWidget>

class QLabel;
class QPushButton;

class ReplaceCurrentWidget : public QWidget
{
	Q_OBJECT

public:
	ReplaceCurrentWidget(QWidget *parent = 0);
	~ReplaceCurrentWidget();
	void setReplacement(const QString &text, const QString &replacement);
	void search(const QString &text, const QString &replacement, bool isCaseSensitive, bool findWholeWords, bool forward, bool startAtCursor);

signals:
	void showReplaceWidget();
	void search(const QString &text, bool isCaseSensitive, bool findWholeWords, bool forward, bool startAtCursor);
	void replace(const QString &replacement);
	void replaceAll(const QString &text, const QString &replacement, bool isCaseSensitive, bool findWholeWords, bool forward, bool startAtCursor);
	void setSearchFromBegin(bool searchFromBegin);

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

protected slots:
	void hide();

private:
	QLabel *m_replaceLabel;
	QPushButton *m_replaceButton;
	QString m_text, m_replacement;
	bool m_isCaseSensitive, m_findWholeWords, m_forward, m_startAtCursor;

private slots:
	void replace();
	void replaceAll();
	void dontReplace();
};

#endif
