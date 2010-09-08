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

#include "templatewidget.h"

#include <QApplication>
#include <QComboBox>
#include <QCompleter>
#include <QDirModel>
#include <QLineEdit>
#include <QFileDialog>
#include <QKeyEvent>
#include <QProcess>
#include <QSettings>

#include "utils/filedialog.h"
#include "utils/icon.h"
#include "utils/lineedit.h"
#include "utils/url.h"

TemplateWidget::TemplateWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.templateCombo->setLineEdit(new LineEdit(this));
	ui.templateCombo->setMinimumContentsLength(20);
	ui.templateChooseButton->setIcon(Icon("document-open"));
	ui.templateReloadButton->setIcon(Icon("view-refresh"));

	QCompleter *completer = new QCompleter(this);
	completer->setModel(new QDirModel(completer));
	completer->setCompletionMode(QCompleter::PopupCompletion);
	ui.templateCombo->setCompleter(completer);

	connect(ui.templateChooseButton, SIGNAL(clicked()),
	        this, SLOT(setTemplateFile()));
	connect(ui.templateEditButton, SIGNAL(clicked()),
	        this, SLOT(editTemplateFile()));
	connect(ui.templateReloadButton, SIGNAL(clicked()),
	        this, SLOT(reloadTemplateFile()));
	connect(ui.templateCombo->lineEdit(), SIGNAL(textChanged(QString)),
	        this, SIGNAL(fileNameChanged(QString)));

	readRecentTemplates();
}

TemplateWidget::~TemplateWidget()
{
	saveRecentTemplates();
}

void TemplateWidget::readRecentTemplates()
{
	QSettings settings(ORGNAME, APPNAME);
	ui.templateCombo->setMaxCount(settings.value("TemplateRecentNumber", 10).toInt());
	const QStringList templateRecentList = settings.value("TemplateRecent").toStringList();
	ui.templateCombo->addItems(templateRecentList);
	const int index = templateRecentList.indexOf(settings.value("TemplateFile").toString());
	ui.templateCombo->setCurrentIndex((index >= 0) ? index : 0);
}

void TemplateWidget::saveRecentTemplates()
{
	QSettings settings(ORGNAME, APPNAME);
	QStringList recentTemplates;
	for (int i = 0; i < ui.templateCombo->count(); ++i)
		recentTemplates << ui.templateCombo->itemText(i);
	settings.setValue("TemplateRecent", recentTemplates);
	settings.setValue("TemplateFile", ui.templateCombo->lineEdit()->text());
}

void TemplateWidget::setFileName(const QString &fileName)
{
	disconnect(ui.templateCombo->lineEdit(), SIGNAL(textChanged(QString)),
	        this, SIGNAL(fileNameChanged(QString)));
	const int index = ui.templateCombo->findText(fileName);
	if (index >= 0) // then remove item in order to re-add it at the top
		ui.templateCombo->removeItem(index);
	ui.templateCombo->insertItem(0, fileName);
	ui.templateCombo->lineEdit()->setText("");
	connect(ui.templateCombo->lineEdit(), SIGNAL(textChanged(QString)),
	        this, SIGNAL(fileNameChanged(QString)));
	ui.templateCombo->setCurrentIndex(0);
}

void TemplateWidget::setReplaceText(const QString &replace)
{
	QString replaceText = replace;
	replaceText.replace('&', QLatin1String("&amp;"));
	replaceText.replace('<', QLatin1String("&lt;"));
	replaceText.replace('>', QLatin1String("&gt;"));
	const QString templateDescription(tr("<p>The template contains the code "
	    "of a complete LaTeX document in which the TikZ picture will be "
	    "included and which will be typesetted to produce the preview "
	    "image.  The string %1 in the template will be replaced by the "
	    "TikZ code.</p>").arg(replaceText));
	ui.templateCombo->setWhatsThis(tr("<p>Give the file name of the LaTeX "
	    "template.  If this input field is empty or contains an invalid "
	    "file name, an internal default template will be used.</p>")
	    + templateDescription);
	ui.templateLabel->setWhatsThis(ui.templateCombo->whatsThis());
	ui.templateEditButton->setWhatsThis(tr("<p>Edit this template with "
	    "an external editor specified in the \"Configure\" dialog.</p>")
	    + templateDescription);
}

void TemplateWidget::setEditor(const QString &editor)
{
	m_editor = editor;
}

QString TemplateWidget::fileName() const
{
	return ui.templateCombo->currentText();
}

void TemplateWidget::setTemplateFile()
{
	QString currentFileName = ui.templateCombo->currentText();
#ifdef KTIKZ_TEMPLATES_INSTALL_DIR
	if (currentFileName.isEmpty() && QFileInfo(KTIKZ_TEMPLATES_INSTALL_DIR).isDir())
		currentFileName = KTIKZ_TEMPLATES_INSTALL_DIR;
#endif
	const Url url = FileDialog::getOpenUrl(this,
	    tr("Select a template file"), Url(currentFileName),
	    QString("*.pgs *.tex|%1\n*|%2")
	    .arg(tr("%1 template files").arg(APPNAME))
	    .arg(tr("All files")));
	if (url.isValid())
		setFileName(url.pathOrUrl());
}

void TemplateWidget::editTemplateFile()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QStringList editorArguments;
	editorArguments << ui.templateCombo->currentText();

	QProcess process;
	process.startDetached(m_editor, editorArguments);

	QApplication::restoreOverrideCursor();

//	reloadTemplateFile();
}

void TemplateWidget::reloadTemplateFile()
{
	setFileName(fileName());
}

void TemplateWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
		setFileName(ui.templateCombo->currentText());
	if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Return)
		emit focusEditor();
	QWidget::keyPressEvent(event);
}
