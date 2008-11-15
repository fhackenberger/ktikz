/***************************************************************************
 *   Copyright (C) 2008 by Glad Deschrijver                                *
 *   Glad.Deschrijver@UGent.be                                             *
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

#include <QApplication>
#include <QLineEdit>
#include <QFileDialog>
#include <QKeyEvent>
#include <QProcess>
#include "templatewidget.h"

TemplateWidget::TemplateWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.templateChooseButton, SIGNAL(clicked()),
	        this, SLOT(setTemplateFile()));
	connect(ui.templateEditButton, SIGNAL(clicked()),
	        this, SLOT(editTemplateFile()));
	connect(ui.templateEdit, SIGNAL(textChanged(const QString&)),
	        this, SIGNAL(fileNameChanged(const QString&)));
}

TemplateWidget::~TemplateWidget()
{
}

void TemplateWidget::setFileName(const QString &fileName)
{
	ui.templateEdit->setText(fileName);
}

void TemplateWidget::setReplaceText(const QString &replace)
{
	QString replaceText = replace;
	replaceText.replace("&", "&amp;");
	replaceText.replace("<", "&lt;");
	replaceText.replace(">", "&gt;");
	ui.templateEditButton->setWhatsThis(tr("<p>Edit this template with "
	    "an external editor specified in the \"Configure\" dialog.</p>")
	    + tr("<p>The string %1 in the template will be replaced by the "
	    "TikZ code.</p>").arg(replaceText));
}

void TemplateWidget::setEditor(const QString &editor)
{
	m_editor = editor;
}

QString TemplateWidget::fileName() const
{
	return ui.templateEdit->text();
}

void TemplateWidget::setTemplateFile()
{
	const QString fileName = QFileDialog::getOpenFileName(this,
	    tr("Select a template file"), ui.templateEdit->text(),
	    QString("%1 (*.pgs *.tex);;%2 (*.*)")
	    .arg(tr("KTikZ template files")).arg(tr("All files")));
	if (!fileName.isEmpty())
		ui.templateEdit->setText(fileName);
}

void TemplateWidget::editTemplateFile()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QStringList editorArguments;
	editorArguments << ui.templateEdit->text();

	QProcess process;
	process.startDetached(m_editor, editorArguments);

	QApplication::restoreOverrideCursor();
}

void TemplateWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Return)
		emit focusEditor();
	QWidget::keyPressEvent(event);
}
