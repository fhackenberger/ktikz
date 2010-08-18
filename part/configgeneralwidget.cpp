/***************************************************************************
 *   Copyright (C) 2008, 2009 by Glad Deschrijver                          *
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

#include <QSettings>

PartConfigGeneralWidget::PartConfigGeneralWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.verticalLayout->setContentsMargins(0, 0, 0, 0);
}

void PartConfigGeneralWidget::setDefaults()
{
	ui.latexUrlRequester->setText("pdflatex");
	ui.pdftopsUrlRequester->setText("pdftops");
	ui.editorUrlRequester->setText("kwrite");
	ui.replaceEdit->setText("<>");
}

void PartConfigGeneralWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup(settingsGroup);
	ui.latexUrlRequester->setText(settings.value("LatexCommand", "pdflatex").toString());
	ui.pdftopsUrlRequester->setText(settings.value("PdftopsCommand", "pdftops").toString());
	ui.editorUrlRequester->setText(settings.value("TemplateEditor", "kwrite").toString());
	ui.replaceEdit->setText(settings.value("TemplateReplaceText", "<>").toString());
	settings.endGroup();

	connect(ui.latexUrlRequester, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
	connect(ui.pdftopsUrlRequester, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
	connect(ui.editorUrlRequester, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
	connect(ui.replaceEdit, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
}

void PartConfigGeneralWidget::setModified()
{
	QWidget *sendingWidget = qobject_cast<QWidget*>(sender());
	QSettings settings(ORGNAME, APPNAME);
	if (sendingWidget->objectName() == QLatin1String("latexUrlRequester"))
		emit changed(ui.latexUrlRequester->text() != settings.value("LatexCommand", "pdflatex").toString());
	else if (sendingWidget->objectName() == QLatin1String("pdftopsUrlRequester"))
		emit changed(ui.pdftopsUrlRequester->text() != settings.value("PdftopsCommand", "pdftops").toString());
	else if (sendingWidget->objectName() == QLatin1String("editorUrlRequester"))
		emit changed(ui.editorUrlRequester->text() != settings.value("TemplateEditor", "kwrite").toString());
	else if (sendingWidget->objectName() == QLatin1String("replaceEdit"))
		emit changed(ui.replaceEdit->text() != settings.value("TemplateReplaceText", "<>").toString());
}

void PartConfigGeneralWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup(settingsGroup);
	settings.setValue("LatexCommand", ui.latexUrlRequester->text());
	settings.setValue("PdftopsCommand", ui.pdftopsUrlRequester->text());
	settings.setValue("TemplateEditor", ui.editorUrlRequester->text());
	settings.setValue("TemplateReplaceText", ui.replaceEdit->text());
	settings.endGroup();
}
