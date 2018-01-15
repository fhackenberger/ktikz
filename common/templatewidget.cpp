/***************************************************************************
 *   Copyright (C) 2008, 2010, 2011, 2012, 2014                            *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
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

#ifdef KTIKZ_USE_KDE
#include <KRun>
#endif

#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtGui/QKeyEvent>
#include <QtGui/QTextDocument>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#else
#include <QtGui/QApplication>
#endif

#include "utils/combobox.h"
#include "utils/filedialog.h"
#include "utils/icon.h"
#include "utils/lineedit.h"
#include "utils/url.h"
#include "utils/urlcompletion.h"

TemplateWidget::TemplateWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
	ui.templateCombo->setEditable(true);
	ui.templateCombo->lineEdit()->setClearButtonEnabled(true);
#else
	ui.templateCombo->setLineEdit(new LineEdit(this)); // slow
#endif
	ui.templateCombo->setMinimumContentsLength(20);
	ui.templateChooseButton->setIcon(Icon(QLatin1String("document-open")));
#ifdef KTIKZ_KPART
	ui.templateReloadButton->setVisible(false);
#else
	ui.templateReloadButton->setIcon(Icon(QLatin1String("view-refresh")));
#endif
	ui.templateEditButton->setIcon(Icon(QLatin1String("document-edit")));

	m_urlCompletion = new UrlCompletion(this);
	ui.templateCombo->setCompletionObject(m_urlCompletion);

	connect(ui.templateChooseButton, SIGNAL(clicked()),
	        this, SLOT(selectTemplateFile()));
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
	delete m_urlCompletion;
}

QWidget *TemplateWidget::lastTabOrderWidget()
{
	return ui.templateEditButton;
}

void TemplateWidget::readRecentTemplates()
{
	QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
	ui.templateCombo->setMaxCount(settings.value(QLatin1String("TemplateRecentNumber"), 10).toInt());
	const QStringList templateRecentList = settings.value(QLatin1String("TemplateRecent")).toStringList();
	ui.templateCombo->addItems(templateRecentList);
	const int index = templateRecentList.indexOf(settings.value(QLatin1String("TemplateFile")).toString());
	ui.templateCombo->setCurrentIndex(index >= 0 ? index : 0);
}

void TemplateWidget::saveRecentTemplates()
{
	QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
	QStringList recentTemplates;
	for (int i = 0; i < ui.templateCombo->count(); ++i)
		recentTemplates << ui.templateCombo->itemText(i);
	settings.setValue(QLatin1String("TemplateRecent"), recentTemplates);
	settings.setValue(QLatin1String("TemplateFile"), ui.templateCombo->lineEdit()->text());
}

void TemplateWidget::setFileName(const QString &fileName)
{
	disconnect(ui.templateCombo->lineEdit(), SIGNAL(textChanged(QString)),
	           this, SIGNAL(fileNameChanged(QString)));
	const int index = ui.templateCombo->findText(fileName);
	if (index >= 0) // then remove item in order to re-add it at the top
		ui.templateCombo->removeItem(index);
	ui.templateCombo->insertItem(0, fileName);
	ui.templateCombo->lineEdit()->setText(QString());
	connect(ui.templateCombo->lineEdit(), SIGNAL(textChanged(QString)),
	        this, SIGNAL(fileNameChanged(QString)));
	ui.templateCombo->setCurrentIndex(0);
}

void TemplateWidget::setReplaceText(const QString &replace)
{
	const QString templateDescription(tr("<p>The template contains the code "
	                                     "of a complete LaTeX document in which the TikZ picture will be "
	                                     "included and which will be typesetted to produce the preview "
	                                     "image.  The string %1 in the template will be replaced by the "
	                                     "TikZ code.</p>")
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	                                     .arg(replace.toHtmlEscaped()));
#else
	                                     .arg(Qt::escape(replace)));
#endif
	ui.templateCombo->setWhatsThis(tr("<p>Give the file name of the LaTeX "
	                                  "template.  If this input field is empty or contains an invalid "
	                                  "file name, an internal default template will be used.</p>")
	                               + templateDescription);
	ui.templateLabel->setWhatsThis(ui.templateCombo->whatsThis());
	ui.templateEditButton->setWhatsThis(tr("<p>Edit this template with "
	                                       "an external editor specified in the \"Configure\" dialog.</p>")
	                                    + templateDescription);
#ifdef KTIKZ_KPART
	// dirty hack: make the following strings translated in the kpart:
	ui.templateLabel->setText(tr("&Template:"));
	ui.templateChooseButton->setWhatsThis(tr("<p>Browse to an existing template file.</p>"));
	ui.templateChooseButton->setToolTip(tr("Select template file"));
	ui.templateEditButton->setToolTip(tr("Edit template file"));
#endif
}

void TemplateWidget::setEditor(const QString &editor)
{
	m_editor = editor;
}

QString TemplateWidget::fileName() const
{
	return ui.templateCombo->currentText();
}

void TemplateWidget::selectTemplateFile()
{
	QString currentFileName = ui.templateCombo->currentText();
#ifdef KTIKZ_TEMPLATES_INSTALL_DIR
	if (currentFileName.isEmpty() && QFileInfo(QString::fromLocal8Bit(KTIKZ_TEMPLATES_INSTALL_DIR)).isDir())
		currentFileName = QString::fromLocal8Bit(KTIKZ_TEMPLATES_INSTALL_DIR);
#endif
	const Url url = FileDialog::getOpenUrl(this,
	                                       tr("Select a template file"), Url(currentFileName),
	                                       QString(QLatin1String("*.pgs *.tex|%1\n*|%2"))
	                                       .arg(tr("%1 template files").arg(QCoreApplication::applicationName()))
	                                       .arg(tr("All files")));
	if (url.isValid())
		setFileName(url.pathOrUrl());
}

void TemplateWidget::editTemplateFile()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QStringList editorArguments;
	editorArguments << ui.templateCombo->currentText();

#ifdef KTIKZ_USE_KDE
	KRun::runUrl( Url( fileName() ), QStringLiteral( "text/plain" ), NULL,  0 );
#else
	QProcess process;
	process.startDetached(m_editor, editorArguments);
#endif

	QApplication::restoreOverrideCursor();
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
		Q_EMIT focusEditor();
	QWidget::keyPressEvent(event);
}
