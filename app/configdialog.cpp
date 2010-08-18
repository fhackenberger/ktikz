/***************************************************************************
 *   Copyright (C) 2007, 2008, 2009 by Glad Deschrijver                    *
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

#include "configdialog.h"

#ifndef KTIKZ_USE_KDE
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QListWidget>
#include <QStackedWidget>
//#include <QTabWidget>
#include <QVBoxLayout>
#endif
#include <QKeyEvent>
#include <QWhatsThis>

#include "configgeneralwidget.h"
#include "configeditorwidget.h"
#include "configappearancewidget.h"
#include "ktikzapplication.h"
#include "../common/utils/icon.h"

#ifdef KTIKZ_USE_KDE
ConfigDialog::ConfigDialog(QWidget *parent) : KPageDialog(parent)
{
	setFaceType(List);
	setCaption(tr("Configure %1").arg(KtikzApplication::applicationName()));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::Help);
	setHelp("chap-configuration");

	addPage(generalPage(), tr("&General"), "preferences-desktop-theme");
	addPage(editorPage(), tr("&Editor"), "accessories-text-editor");
	addPage(appearancePage(), tr("&Highlighting"), "preferences-desktop-color");

	connect(this, SIGNAL(applyClicked()), this, SLOT(accept()));
	connect(this, SIGNAL(okClicked()), this, SLOT(accept()));
}
#else
ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Configure %1").arg(KtikzApplication::applicationName()));

	addPage(generalPage(), tr("&General"), "preferences-desktop-theme");
	addPage(editorPage(), tr("&Editor"), "accessories-text-editor");
	addPage(appearancePage(), tr("&Highlighting"), "preferences-desktop-color");

	QDialogButtonBox *buttonBox = new QDialogButtonBox;
	QAction *whatsThisAction = QWhatsThis::createAction(this);
	whatsThisAction->setIcon(Icon("help-contextual"));
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

QWidget *ConfigDialog::centerWidget()
{
/*
	m_pagesTabWidget = new QTabWidget;
	for (int i = 0; i < m_pageWidgets.size(); ++i)
		m_pagesTabWidget->addTab(m_pageWidgets.at(i), m_pageTitles.at(i));
	return m_pagesTabWidget;
*/
	// create list
	QListWidget *pagesListWidget = new QListWidget;
	pagesListWidget->setViewMode(QListView::IconMode);
	pagesListWidget->setMovement(QListView::Static);
	pagesListWidget->setFlow(QListView::TopToBottom);
	pagesListWidget->setWordWrap(true);
	pagesListWidget->setUniformItemSizes(true);
	pagesListWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

	// add items to list
	QFontMetrics fm(qApp->font());
	int iconWidth = 0;
	for (int i = 0; i < m_pagesListWidgetItems.size(); ++i)
		iconWidth = qMax(iconWidth, fm.boundingRect(0, 0, 0, 0, Qt::AlignCenter, m_pagesListWidgetItems.at(i)->text()).width());
	iconWidth += 20;
	const int iconHeight = fm.height() + 42;
	for (int i = 0; i < m_pagesListWidgetItems.size(); ++i)
	{
		m_pagesListWidgetItems.at(i)->setSizeHint(QSize(iconWidth, iconHeight));
		pagesListWidget->addItem(m_pagesListWidgetItems.at(i));
	}
	pagesListWidget->setFixedWidth(m_pagesListWidgetItems.at(0)->sizeHint().width() + 6);

	// create title
	QFrame *titleFrame = new QFrame(this);
//	titleFrame->setAutoFillBackground(true);
//	titleFrame->setFrameShape(QFrame::StyledPanel);
	titleFrame->setFrameShape(QFrame::Box);
//	titleFrame->setFrameShadow(QFrame::Plain);
//	titleFrame->setBackgroundRole(QPalette::Base);
	m_pagesTitleLabel = new QLabel(titleFrame);
	m_pagesTitleLabel->setStyleSheet("QLabel { font-weight: bold; }");
	QGridLayout *titleLayout = new QGridLayout(titleFrame);
	titleLayout->setColumnStretch(0, 1);
	titleLayout->setMargin(6);
	titleLayout->addWidget(m_pagesTitleLabel);

	// add pages
	QStackedWidget *pagesStackedWidget = new QStackedWidget;
	for (int i = 0; i < m_pageWidgets.size(); ++i)
		pagesStackedWidget->addWidget(m_pageWidgets.at(i));
	connect(pagesListWidget, SIGNAL(currentRowChanged(int)), pagesStackedWidget, SLOT(setCurrentIndex(int)));
	connect(pagesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentPage(int)));
	pagesListWidget->setCurrentRow(0);

	QWidget *mainWidget = new QWidget;
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(pagesListWidget, 0, 0, 2, 1);
	mainLayout->addWidget(titleFrame, 0, 1);
	mainLayout->addWidget(pagesStackedWidget, 1, 1);
	mainWidget->setLayout(mainLayout);

	return mainWidget;
}
#endif

void ConfigDialog::addPage(QWidget *widget, const QString &title, const QString &iconName)
{
/*
	Q_UNUSED(iconName);
	m_pageTitles << title;
	m_pageWidgets << widget;
*/
	QString title2 = title;
	title2.remove('&');
#ifdef KTIKZ_USE_KDE
	KPageWidgetItem *page = new KPageWidgetItem(widget, title2);
	page->setHeader(title2);
	page->setIcon(KIcon(iconName));
	KPageDialog::addPage(page);
#else
	QListWidgetItem *item = new QListWidgetItem(Icon(iconName), title2);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	m_pagesListWidgetItems << item;

	m_pageWidgets << widget;
#endif
}

#ifndef KTIKZ_USE_KDE
void ConfigDialog::setCurrentPage(int page)
{
	m_pagesTitleLabel->setText(m_pagesListWidgetItems.at(page)->text());
}
#endif

QWidget *ConfigDialog::generalPage()
{
	m_configGeneralWidget = new ConfigGeneralWidget(this);
	return m_configGeneralWidget;
}

QWidget *ConfigDialog::editorPage()
{
	m_configEditorWidget = new ConfigEditorWidget(this);
	return m_configEditorWidget;
}

QWidget *ConfigDialog::appearancePage()
{
	m_configAppearanceWidget = new ConfigAppearanceWidget(this);
	return m_configAppearanceWidget;
}

void ConfigDialog::readSettings()
{
	m_configGeneralWidget->readSettings("");
	m_configEditorWidget->readSettings("Editor");
	m_configAppearanceWidget->readSettings("Highlighting");
}

void ConfigDialog::writeSettings()
{
	m_configGeneralWidget->writeSettings("");
	m_configEditorWidget->writeSettings("Editor");
	m_configAppearanceWidget->writeSettings("Highlighting");
}

void ConfigDialog::setTranslatedHighlightTypeNames(const QStringList &typeNames)
{
	for (int i = 0; i < typeNames.size(); ++i)
		m_configAppearanceWidget->addItem(typeNames.at(i));
}

void ConfigDialog::setHighlightTypeNames(const QStringList &typeNames)
{
	m_configAppearanceWidget->setTypeNames(typeNames);
}

void ConfigDialog::setDefaultHighlightFormats(const QMap<QString, QTextCharFormat> &defaultFormatList)
{
	m_configAppearanceWidget->setDefaultTextCharFormats(defaultFormatList);
}

void ConfigDialog::accept()
{
	writeSettings();
	emit settingsChanged();
	QDialog::accept();
}

void ConfigDialog::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_F1 && event->modifiers() == Qt::ShiftModifier)
	{
		QWhatsThis::enterWhatsThisMode();
	}
	QDialog::keyPressEvent(event);
}
