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

#include "editreplacecurrentwidget.h"

#include <QtGui/QKeyEvent>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#endif

#include "../common/utils/icon.h"

ReplaceCurrentWidget::ReplaceCurrentWidget(QWidget *parent) : QWidget(parent)
{
	m_replaceLabel = new QLabel;
	m_replaceButton = new QPushButton(tr("&Replace"));
	QPushButton *replaceAllButton = new QPushButton(tr("Replace &All"));
	QPushButton *dontReplaceButton = new QPushButton(tr("&Don't Replace"));
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
	cancelButton->setIcon(Icon(QLatin1String("dialog-cancel")));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QWidget *buttonsWidget = new QWidget;
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(m_replaceButton);
	buttonsLayout->addWidget(replaceAllButton);
	buttonsLayout->addWidget(dontReplaceButton);
	buttonsLayout->addWidget(cancelButton);
	buttonsLayout->addStretch();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
#else
    buttonsLayout->setMargin(0);
#endif
	buttonsWidget->setLayout(buttonsLayout);
	mainLayout->addWidget(m_replaceLabel);
	mainLayout->addWidget(buttonsWidget);

	setFocusProxy(m_replaceButton);

	connect(m_replaceButton, SIGNAL(clicked()), this, SIGNAL(replace()));
	connect(replaceAllButton, SIGNAL(clicked()), this, SIGNAL(replaceAll()));
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

void ReplaceCurrentWidget::dontReplace()
{
	Q_EMIT search();
}

void ReplaceCurrentWidget::hide()
{
	setVisible(false);
	Q_EMIT hidden();
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
		Q_EMIT replace();
	QWidget::keyPressEvent(event);
}
