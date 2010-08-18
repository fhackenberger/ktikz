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

#include "editreplacecurrentwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QPushButton>
#include <QVBoxLayout>

#include "../common/utils/icon.h"

ReplaceCurrentWidget::ReplaceCurrentWidget(QWidget *parent) : QWidget(parent)
{
	m_replaceLabel = new QLabel;
	m_replaceButton = new QPushButton(tr("&Replace"));
	QPushButton *replaceAllButton = new QPushButton(tr("Replace &All"));
	QPushButton *dontReplaceButton = new QPushButton(tr("&Don't Replace"));
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
	cancelButton->setIcon(Icon("dialog-cancel"));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QWidget *buttonsWidget = new QWidget;
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(m_replaceButton);
	buttonsLayout->addWidget(replaceAllButton);
	buttonsLayout->addWidget(dontReplaceButton);
	buttonsLayout->addWidget(cancelButton);
	buttonsLayout->addStretch();
	buttonsLayout->setMargin(0);
	buttonsWidget->setLayout(buttonsLayout);
	mainLayout->addWidget(m_replaceLabel);
	mainLayout->addWidget(buttonsWidget);

	setFocusProxy(m_replaceButton);

	connect(m_replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
	connect(replaceAllButton, SIGNAL(clicked()), this, SLOT(replaceAll()));
	connect(dontReplaceButton, SIGNAL(clicked()), this, SLOT(dontReplace()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
}

ReplaceCurrentWidget::~ReplaceCurrentWidget()
{
}

void ReplaceCurrentWidget::setReplacement(const QString &text, const QString &replacement)
{
	m_replaceLabel->setText(tr("Replace %1 by %2?").arg(text).arg(replacement));
}

void ReplaceCurrentWidget::search(const QString &text, const QString &replacement, bool isCaseSensitive, bool findWholeWords, bool forward, bool startAtCursor)
{
	m_text = text;
	m_replacement = replacement;
	m_isCaseSensitive = isCaseSensitive;
	m_findWholeWords = findWholeWords;
	m_forward = forward;
	m_startAtCursor = startAtCursor;
	emit search(m_text, m_isCaseSensitive, m_findWholeWords, m_forward, m_startAtCursor);
}

void ReplaceCurrentWidget::replace()
{
	emit replace(m_replacement);
	emit setSearchFromBegin(false);
	emit search(m_text, m_isCaseSensitive, m_findWholeWords, m_forward, m_startAtCursor);
}

void ReplaceCurrentWidget::replaceAll()
{
	emit replace(m_replacement);
	emit replaceAll(m_text, m_replacement, m_isCaseSensitive, m_findWholeWords, m_forward, m_startAtCursor);
}

void ReplaceCurrentWidget::dontReplace()
{
	emit setSearchFromBegin(false);
	emit search(m_text, m_isCaseSensitive, m_findWholeWords, m_forward, m_startAtCursor);
}

void ReplaceCurrentWidget::hide()
{
	setVisible(false);
	emit showReplaceWidget();
}

void ReplaceCurrentWidget::showEvent(QShowEvent *event)
{
	m_replaceButton->setFocus();
	QWidget::showEvent(event);
}

void ReplaceCurrentWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
		hide();
	else if (event->key() == Qt::Key_Return)
		replace();
	QWidget::keyPressEvent(event);
}
