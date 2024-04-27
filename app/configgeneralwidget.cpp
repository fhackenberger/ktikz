/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012 by Glad Deschrijver        *
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

#include "configgeneralwidget.h"

#include <QtCore/QSettings>

#include "ktikzapplication.h"
#include "tikzdocumentationcontroller.h"
#include "../common/utils/filedialog.h"
#include "../common/utils/icon.h"
#include "../common/utils/messagebox.h"
#include "../common/utils/urlcompletion.h"
#include "../common/utils/url.h"

ConfigGeneralWidget::ConfigGeneralWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

#ifdef KTIKZ_USE_KDE
    ui.historyLengthLabel->setVisible(false);
    ui.historyLengthSpinBox->setVisible(false);
    ui.toolBarStyleLabel->setVisible(false);
    ui.toolBarStyleComboBox->setVisible(false);
#endif

#ifdef KTIKZ_USE_KTEXTEDITOR
    ui.editorWidgetComboBox->addItem(tr("Qt Editor", "TextEditor"), QString());
    ui.editorWidgetComboBox->addItem(tr("KDE Frameworks Editor", "TextEditor"), QString());
#endif

    m_urlCompletion = new UrlCompletion(this);
    ui.tikzDocEdit->setCompletionObject(m_urlCompletion);
    ui.latexEdit->setCompletionObject(m_urlCompletion);
    ui.pdftopsEdit->setCompletionObject(m_urlCompletion);
    ui.editorEdit->setCompletionObject(m_urlCompletion);

    ui.tikzDocButton->setIcon(Icon(QLatin1String("document-open")));
    ui.latexButton->setIcon(Icon(QLatin1String("document-open")));
    ui.pdftopsButton->setIcon(Icon(QLatin1String("document-open")));
    ui.editorButton->setIcon(Icon(QLatin1String("document-open")));

    connect(ui.tikzDocButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
    connect(ui.tikzDocSearchButton, SIGNAL(clicked()), this, SLOT(searchTikzDocumentation()));
    connect(ui.latexButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
    connect(ui.pdftopsButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
    connect(ui.editorButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
}

ConfigGeneralWidget::~ConfigGeneralWidget()
{
    delete m_urlCompletion;
}

void ConfigGeneralWidget::readSettings(const QString &settingsGroup)
{
    QSettings settings;
    settings.beginGroup(settingsGroup);
#ifndef KTIKZ_USE_KDE
    ui.historyLengthSpinBox->setValue(
            settings.value(QLatin1String("RecentFilesNumber"), 10).toInt());
#endif
#ifdef KTIKZ_USE_KTEXTEDITOR
    ui.editorWidgetComboBox->setCurrentIndex(
            settings.value(QLatin1String("EditorWidget"), 0).toInt());
#endif
    ui.commandsInDockCheck->setChecked(
            settings.value(QLatin1String("CommandsInDock"), false).toBool());
    ui.tikzDocEdit->setText(TikzDocumentationController::tikzDocumentationPath());
    ui.latexEdit->setText(
            settings.value(QLatin1String("LatexCommand"), QLatin1String("pdflatex")).toString());
    ui.pdftopsEdit->setText(
            settings.value(QLatin1String("PdftopsCommand"), QLatin1String("pdftops")).toString());
    ui.editorEdit->setText(
            settings.value(QLatin1String("TemplateEditor"), QLatin1String("")).toString());
    ui.replaceEdit->setText(
            settings.value(QLatin1String("TemplateReplaceText"), QLatin1String("<>")).toString());
    settings.endGroup();

#ifndef KTIKZ_USE_KDE
    settings.beginGroup(QLatin1String("MainWindow"));
    ui.toolBarStyleComboBox->setCurrentIndex(
            settings.value(QLatin1String("ToolBarStyle"), 0).toInt());
    settings.endGroup();
#endif
}

void ConfigGeneralWidget::writeSettings(const QString &settingsGroup)
{
    QSettings settings;
    settings.beginGroup(settingsGroup);
#ifndef KTIKZ_USE_KDE
    settings.setValue(QLatin1String("RecentFilesNumber"), ui.historyLengthSpinBox->value());
#endif
#ifdef KTIKZ_USE_KTEXTEDITOR
    settings.setValue(QLatin1String("EditorWidget"), ui.editorWidgetComboBox->currentIndex());
#endif
    settings.setValue(QLatin1String("CommandsInDock"), ui.commandsInDockCheck->isChecked());
    TikzDocumentationController::storeTikzDocumentationPath(ui.tikzDocEdit->text());
    settings.setValue(QLatin1String("LatexCommand"), ui.latexEdit->text());
    settings.setValue(QLatin1String("PdftopsCommand"), ui.pdftopsEdit->text());
    settings.setValue(QLatin1String("TemplateEditor"), ui.editorEdit->text());
    settings.setValue(QLatin1String("TemplateReplaceText"), ui.replaceEdit->text());
    settings.endGroup();

#ifndef KTIKZ_USE_KDE
    settings.beginGroup(QLatin1String("MainWindow"));
    settings.setValue(QLatin1String("ToolBarStyle"), ui.toolBarStyleComboBox->currentIndex());
    settings.endGroup();
#endif
}

void ConfigGeneralWidget::searchTikzDocumentation()
{
    const QString tikzDocFile = TikzDocumentationController::searchTikzDocumentationInTexTree();
    if (tikzDocFile.isEmpty())
        MessageBox::error(this, tr("Cannot find TikZ documentation."),
                          KtikzApplication::applicationName());
    else
        ui.tikzDocEdit->setText(tikzDocFile);
}

void ConfigGeneralWidget::browseCommand(QLineEdit *lineEdit, bool isProgram)
{
    QString location;
    if (isProgram) {
        Url url = FileDialog::getOpenUrl(this, tr("Browse program"), Url(QDir::rootPath()));
        location = url.path();
    } else {
        const QString oldLocation = lineEdit->text();
        Url url = FileDialog::getOpenUrl(
                this, tr("Browse file"),
                Url(!oldLocation.isEmpty() ? oldLocation : QDir::homePath()));
        location = url.path();
    }
    if (!location.isEmpty()) {
        location.replace(QLatin1Char('\\'), QLatin1Char('/'));
        lineEdit->setText(location);
    }
}

void ConfigGeneralWidget::browseCommand()
{
    QToolButton *button = qobject_cast<QToolButton *>(sender());
    if (button->objectName() == QLatin1String("latexButton"))
        browseCommand(ui.latexEdit);
    else if (button->objectName() == QLatin1String("pdftopsButton"))
        browseCommand(ui.pdftopsEdit);
    else if (button->objectName() == QLatin1String("editorButton"))
        browseCommand(ui.editorEdit);
    else if (button->objectName() == QLatin1String("tikzDocButton"))
        browseCommand(ui.tikzDocEdit, false);
}
