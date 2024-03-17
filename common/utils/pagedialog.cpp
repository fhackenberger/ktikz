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

#include "pagedialog.h"
#ifdef KTIKZ_USE_KDE
#  include <QPushButton>

PageDialog::PageDialog(QWidget *parent) : KPageDialog(parent)
{
    setFaceType(List);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help
                       | QDialogButtonBox::Apply);
    QPushButton *okButton = buttonBox()->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox()->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(accept()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void PageDialog::addPage(QWidget *widget, const QString &title, const QString &iconName)
{
    QString titleString = title;
    titleString.remove(QLatin1Char('&'));

    KPageWidgetItem *page = new KPageWidgetItem(widget, titleString);
    page->setHeader(titleString);
    page->setIcon(QIcon::fromTheme(iconName));
    KPageDialog::addPage(page);
}
#else
#  include <QAction>
#  include <QApplication>
#  include <QDialogButtonBox>
#  include <QGridLayout>
#  include <QLabel>
#  include <QListWidget>
#  include <QStackedWidget>
#  include <QToolButton>
#  include <QWhatsThis>

#  include <QDialogButtonBox>
#  include <QPushButton>
#  include <QVBoxLayout>

#  include "icon.h"

PageDialog::PageDialog(QWidget *parent) : QDialog(parent)
{
    m_iconWidth = 0;

    // add What's this, OK, Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    QAction *whatsThisAction = QWhatsThis::createAction(this);
    whatsThisAction->setIcon(Icon(QLatin1String("help-contextual")));
    QToolButton *whatsThisButton = new QToolButton(this);
    whatsThisButton->setDefaultAction(whatsThisAction);
    whatsThisButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    buttonBox->addButton(whatsThisButton, QDialogButtonBox::HelpRole);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(centerWidget());
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

QWidget *PageDialog::centerWidget()
{
    // create list
    m_pagesListWidget = new QListWidget;
    m_pagesListWidget->setViewMode(QListView::IconMode);
    m_pagesListWidget->setMovement(QListView::Static);
    m_pagesListWidget->setFlow(QListView::TopToBottom);
    m_pagesListWidget->setWordWrap(true);
    m_pagesListWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // create title
    QFrame *titleFrame = new QFrame(this);
    titleFrame->setFrameShape(QFrame::Box);
    m_pagesTitleLabel = new QLabel(titleFrame);
    m_pagesTitleLabel->setStyleSheet(QLatin1String("QLabel { font-weight: bold; }"));
    QGridLayout *titleLayout = new QGridLayout(titleFrame);
    titleLayout->setColumnStretch(0, 1);
    titleLayout->setMargin(6);
    titleLayout->addWidget(m_pagesTitleLabel);

    // add pages
    m_pagesStackedWidget = new QStackedWidget;
    connect(m_pagesListWidget, SIGNAL(currentRowChanged(int)), m_pagesStackedWidget,
            SLOT(setCurrentIndex(int)));
    connect(m_pagesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentPage(int)));

    QWidget *mainWidget = new QWidget;
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(m_pagesListWidget, 0, 0, 2, 1);
    mainLayout->addWidget(titleFrame, 0, 1);
    mainLayout->addWidget(m_pagesStackedWidget, 1, 1);
    mainWidget->setLayout(mainLayout);

    return mainWidget;
}

void PageDialog::setCaption(const QString &caption)
{
    setWindowTitle(caption);
}

void PageDialog::setHelp(const QString &anchor)
{
    Q_UNUSED(anchor);
    // not used in Qt-only version
}

void PageDialog::addPage(QWidget *widget, const QString &title, const QString &iconName)
{
    QString titleString = title;
    titleString.remove(QLatin1Char('&'));

    QListWidgetItem *item = new QListWidgetItem(Icon(iconName), titleString);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_pagesListWidget->addItem(item);
    m_pagesListWidgetItems << item;

    m_iconWidth = qMax(m_iconWidth, m_pagesListWidget->visualItemRect(item).width() + 6);
    m_pagesListWidget->setFixedWidth(m_iconWidth);
    m_pagesListWidget->setCurrentRow(0);

    // center all labels and icons
    for (int i = 0; i < m_pagesListWidget->count(); ++i) {
        QListWidgetItem *item = m_pagesListWidget->item(i);
        item->setSizeHint(QSize(m_pagesListWidget->sizeHintForColumn(0),
                                m_pagesListWidget->visualItemRect(item).height()));
    }

    m_pagesStackedWidget->addWidget(widget);
}

void PageDialog::setCurrentPage(int page)
{
    m_pagesTitleLabel->setText(m_pagesListWidgetItems.at(page)->text());
}
#endif
