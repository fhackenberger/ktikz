/***************************************************************************
 *   Copyright (C) 2008-2009 by Glad Deschrijver                           *
 *   glad.deschrijver@gmail.com                                            *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "configgeneralwidget.h"
#ifdef KTIKZ_USE_KDE
#include <KLocalizedString>
#include <KMessageBox>
#else
#include <QMessageBox>
#endif

#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>
#include <QProcess>
#include <QSettings>

#include "ktikzapplication.h"

ConfigGeneralWidget::ConfigGeneralWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
/*
	ui.url_LatexCommand->lineEdit()->setObjectName("kcfg_LatexCommand");
	ui.url_LatexCommand->fileDialog()->setCaption(i18nc("@title:window", "Select LaTeX Executable"));
	ui.url_PdftopsCommand->lineEdit()->setObjectName("kcfg_PdftopsCommand");
	ui.url_PdftopsCommand->fileDialog()->setCaption(i18nc("@title:window", "Select Pdftops Executable"));
	ui.url_TikzDocumentation->lineEdit()->setObjectName("kcfg_TikzDocumentation");
	ui.url_TikzDocumentation->fileDialog()->setCaption(i18nc("@title:window", "Select TikZ documentation file"));
*/
	connect(ui.commandsInDockCheck, SIGNAL(toggled(bool)), this, SLOT(setCommandsInDock(bool)));

	QCompleter *completer = new QCompleter(this);
	completer->setModel(new QDirModel(completer));
	completer->setCompletionMode(QCompleter::PopupCompletion);
	ui.tikzDocEdit->setCompleter(completer);
	ui.latexEdit->setCompleter(completer);
	ui.pdftopsEdit->setCompleter(completer);
	ui.editorEdit->setCompleter(completer);

#ifdef KTIKZ_USE_KDE
	ui.tikzDocButton->setIcon(KIcon("document-open"));
	ui.latexButton->setIcon(KIcon("document-open"));
	ui.pdftopsButton->setIcon(KIcon("document-open"));
	ui.editorButton->setIcon(KIcon("document-open"));
#endif
	connect(ui.tikzDocButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
	connect(ui.tikzDocSearchButton, SIGNAL(clicked()), this, SLOT(searchTikzDocumentation()));
	connect(ui.latexButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
	connect(ui.pdftopsButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
	connect(ui.editorButton, SIGNAL(clicked()), this, SLOT(browseCommand()));
}

void ConfigGeneralWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	ui.historyLengthSpinBox->setValue(settings.value("RecentFilesNumber", 10).toInt());
	ui.commandsInDockCheck->setChecked(settings.value("CommandsInDock", false).toBool());
	ui.tikzDocEdit->setText(settings.value("TikzDocumentation").toString());
	ui.latexEdit->setText(settings.value("LatexCommand", "pdflatex").toString());
	ui.pdftopsEdit->setText(settings.value("PdftopsCommand", "pdftops").toString());
	ui.editorEdit->setText(settings.value("TemplateEditor", "kwrite").toString());
	ui.replaceEdit->setText(settings.value("TemplateReplaceText", "<>").toString());
	settings.endGroup();

	settings.beginGroup("MainWindow");
	ui.toolBarStyleComboBox->setCurrentIndex(settings.value("ToolBarStyle", 0).toInt());
	settings.endGroup();
}

void ConfigGeneralWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	settings.setValue("RecentFilesNumber", ui.historyLengthSpinBox->value());
	settings.setValue("CommandsInDock", ui.commandsInDockCheck->isChecked());
	settings.setValue("TikzDocumentation", ui.tikzDocEdit->text());
	settings.setValue("LatexCommand", ui.latexEdit->text());
	settings.setValue("PdftopsCommand", ui.pdftopsEdit->text());
	settings.setValue("TemplateEditor", ui.editorEdit->text());
	settings.setValue("TemplateReplaceText", ui.replaceEdit->text());
	settings.endGroup();

	settings.beginGroup("MainWindow");
	settings.setValue("ToolBarStyle", ui.toolBarStyleComboBox->currentIndex());
	settings.endGroup();
}

void ConfigGeneralWidget::searchTikzDocumentation()
{
	const QString kpsewhichCommand = "kpsewhich";
	QStringList kpsewhichArguments;
	kpsewhichArguments << "--format" << "TeX system documentation" << "pgfmanual.pdf";

	QProcess process;
	process.start(kpsewhichCommand, kpsewhichArguments);
	process.waitForStarted(1000);
	while (process.state() != QProcess::NotRunning)
		process.waitForFinished(100 /*msec*/);

	QString tikzDocFile = process.readAllStandardOutput();
	tikzDocFile = tikzDocFile.trimmed();
	if (tikzDocFile.isEmpty())
#ifdef KTIKZ_USE_KDE
		KMessageBox::sorry(this, i18nc("@info", "Cannot find TikZ documentation."));
#else
		QMessageBox::warning(this, KtikzApplication::applicationName(),
		                     tr("Cannot find TikZ documentation."));
#endif
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
		location.replace("\\", "/");
		lineEdit->setText(location);
	}
}

void ConfigGeneralWidget::browseCommand()
{
	QToolButton *button = qobject_cast<QToolButton*>(sender());
	if (button->objectName() == "latexButton")
		browseCommand(ui.latexEdit);
	else if (button->objectName() == "pdftopsButton")
		browseCommand(ui.pdftopsEdit);
	else if (button->objectName() == "editorButton")
		browseCommand(ui.editorEdit);
	else if (button->objectName() == "tikzDocButton")
		browseCommand(ui.tikzDocEdit, false);
}
