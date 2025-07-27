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

#include "tikzpreviewmessagewidget.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#ifdef KTIKZ_USE_KDE
#  include <KIconLoader>
#endif

#include "utils/icon.h"

TikzPreviewMessageWidget::TikzPreviewMessageWidget(QWidget *parent) : QFrame(parent)
{
#ifdef KTIKZ_USE_KDE
    const QPixmap infoPixmap = KIconLoader::global()->loadIcon(
            QLatin1String("dialog-error"), KIconLoader::Dialog, KIconLoader::SizeMedium);
#else
    const QPixmap infoPixmap = Icon(QLatin1String("dialog-error")).pixmap(QSize(32, 32));
#endif
    m_infoPixmapLabel = new QLabel;
    m_infoPixmapLabel->setPixmap(infoPixmap);

    m_infoLabel = new QLabel;
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    setObjectName(QLatin1String("PreviewMessageWidget"));
    setFrameShape(QFrame::Box);

    setStyleSheet(QString(QLatin1String("QFrame {"
                                        "  background-color: palette(midlight);"
                                        "  border-radius: 5px;"
                                        "  border: 1px solid palette(dark);"
                                        "}"
                                        "QLabel {"
                                        "  border: none;"
                                        "  color: palette(windowText);"
                                        "}")));

    QHBoxLayout *infoLayout = new QHBoxLayout(this);
    infoLayout->setContentsMargins(10, 10, 10, 10);
    infoLayout->addWidget(m_infoPixmapLabel);
    infoLayout->addWidget(m_infoLabel);

    m_infoPixmapLabel->setVisible(false);
}

TikzPreviewMessageWidget::~TikzPreviewMessageWidget() { }

QSize TikzPreviewMessageWidget::calculateSize(bool pixmapVisible) const
{
    if (pixmapVisible) {
        return QSize(m_infoPixmapLabel->sizeHint().width() + m_infoLabel->sizeHint().width() + 35,
                     qMax(m_infoPixmapLabel->sizeHint().height(), m_infoLabel->sizeHint().height())
                             + 25);
    }
    return QSize(m_infoLabel->sizeHint().width() + 25, m_infoLabel->sizeHint().height() + 25);
}

QSize TikzPreviewMessageWidget::sizeHint() const
{
    return calculateSize(m_infoPixmapLabel->isVisible());
}

void TikzPreviewMessageWidget::setText(const QString &message, PixmapVisibility pixmapVisibility)
{
    m_infoPixmapLabel->setVisible(pixmapVisibility == PixmapVisible);
    m_infoLabel->setText(message);
    resize(calculateSize(pixmapVisibility == PixmapVisible));
}
