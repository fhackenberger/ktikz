/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Glad Deschrijver                    *
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

#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>

#include "ktikzapplication.h"
#include "tikzdocumentationcontroller.h"
#include "../common/utils/icon.h"

ConfigGeneralWidget::ConfigGeneralWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

#ifdef KTIKZ_USE_KDE
	ui.historyLengthLabel->setVisible(false);
	ui.historyLengthSpinBox->setVisible(false);
	ui.toolBarStyleLabel->setVisible(false);
	ui.toolBarStyleComboBox->setVisible(false);
#endif

	QCompleter *completer = new QCompleter(this);
	completer->setModel(new QDirModel(completer));
	completer->setCompletionMode(QCompleter::PopupCompletion);
	ui.tikzDocEdit->setCompleter(completer);
	ui.latexEdit->setCompleter(completer);
	ui.pdftopsEdit->setCompleter(completer);
	ui.editorEdit->setCompleter(completer);

	ui.tikzDocButton->setIcon(Icon("document-open"));
	ui.latexButton->setIcon(Icon("document-open"));
	ui.pdftopsButton->setIcon(Icon("document-open"));
	ui.editorButton->setIcon(Icon("document-open"));

	connect(ui.commandsInDockCheck, SIGNAL(toggled(bool)), this, SLOT(setCommandsInDock(bool)));
	connect(ui.tikzDocButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
	connect(ui.tikzDocSearchButton, SIGNAL(clicked()), this, SLOT(searchTikzDocumentation()));
	connect(ui.latexButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
	connect(ui.pdftopsButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
	connect(ui.editorButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
}

void ConfigGeneralWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup(settingsGroup);
#ifndef KTIKZ_USE_KDE
	ui.historyLengthSpinBox->setValue(settings.value("RecentFilesNumber", 10).toInt());
#endif
	ui.commandsInDockCheck->setChecked(settings.value("CommandsInDock", false).toBool());
	if (settings.value("BuildAutomatically", true).toBool())
		ui.buildAutomaticallyRadio->setChecked(true);
	else
		ui.buildManuallyRadio->setChecked(true);
	ui.tikzDocEdit->setText(TikzDocumentationController::tikzDocumentationPath());
	ui.latexEdit->setText(settings.value("LatexCommand", "pdflatex").toString());
	ui.pdftopsEdit->setText(settings.value("PdftopsCommand", "pdftops").toString());
		ui.editorEdit->setText(settings.value("TemplateEditor", KTIKZ_TEMPLATE_EDITOR_DEFAULT).toString());
	ui.replaceEdit->setText(settings.value("TemplateReplaceText", "<>").toString());
	settings.endGroup();

#ifndef KTIKZ_USE_KDE
	settings.beginGroup("MainWindow");
	ui.toolBarStyleComboBox->setCurrentIndex(settings.value("ToolBarStyle", 0).toInt());
	settings.endGroup();
#endif
}

void ConfigGeneralWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup(settingsGroup);
#ifndef KTIKZ_USE_KDE
	settings.setValue("RecentFilesNumber", ui.historyLengthSpinBox->value());
#endif
	settings.setValue("CommandsInDock", ui.commandsInDockCheck->isChecked());
	settings.setValue("BuildAutomatically", ui.buildAutomaticallyRadio->isChecked());
	TikzDocumentationController::storeTikzDocumentationPath(ui.tikzDocEdit->text());
	settings.setValue("LatexCommand", ui.latexEdit->text());
	settings.setValue("PdftopsCommand", ui.pdftopsEdit->text());
	settings.setValue("TemplateEditor", ui.editorEdit->text());
	settings.setValue("TemplateReplaceText", ui.replaceEdit->text());
	settings.endGroup();

#ifndef KTIKZ_USE_KDE
	settings.beginGroup("MainWindow");
	settings.setValue("ToolBarStyle", ui.toolBarStyleComboBox->currentIndex());
	settings.endGroup();
#endif
}

void ConfigGeneralWidget::searchTikzDocumentation()
{
	const QString tikzDocFile = TikzDocumentationController::searchTikzDocumentationInTexTree();
	if (tikzDocFile.isEmpty())
		QMessageBox::warning(this, KtikzApplication::applicationName(),
		                     tr("Cannot find TikZ documentation."));
	else
		ui.tikzDocEdit->setText(tikzDocFile);
}

void ConfigGeneralWidget::browseCommand(QLineEdit *lineEdit, bool isProgram)
{
	QString location;
	if (isProgram)
	{
		location = QFileDialog::getOpenFileName(this,
		    tr("Browse program"), QDir::rootPath(),
		    QString("%1 (*)").arg(tr("Program")), 0,
		    QFileDialog::DontResolveSymlinks);
	}
	else
	{
		const QString oldLocation = lineEdit->text();
		location = QFileDialog::getOpenFileName(this,
		    tr("Browse file"),
		    (!oldLocation.isEmpty()) ? oldLocation : QDir::homePath(),
		    QString("%1 (*.*)").arg(tr("All files")));
	}
	if (!location.isEmpty())
	{
		location.replace('\\', '/');
		lineEdit->setText(location);
	}
}

void ConfigGeneralWidget::browseCommand()
{
	QToolButton *button = qobject_cast<QToolButton*>(sender());
	if (button->objectName() == QLatin1String("latexButton"))
		browseCommand(ui.latexEdit);
	else if (button->objectName() == QLatin1String("pdftopsButton"))
		browseCommand(ui.pdftopsEdit);
	else if (button->objectName() == QLatin1String("editorButton"))
		browseCommand(ui.editorEdit);
	else if (button->objectName() == QLatin1String("tikzDocButton"))
		browseCommand(ui.tikzDocEdit, false);
}
