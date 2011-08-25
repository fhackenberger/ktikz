/***************************************************************************
 *   Copyright (C) 2008, 2011 by Glad Deschrijver                          *
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

#include "editreplacewidget.h"

#ifdef KTIKZ_USE_KDE
#include <KCompletion>
#endif
#include <QtGui/QKeyEvent>

#include "../common/utils/icon.h"
#include "../common/utils/lineedit.h"

ReplaceWidget::ReplaceWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.comboBoxFind->setLineEdit(new LineEdit(this));
	ui.comboBoxReplace->setLineEdit(new LineEdit(this));
	ui.pushButtonClose->setIcon(Icon("dialog-cancel"));
	ui.pushButtonBackward->setIcon(Icon("go-up"));
	ui.pushButtonForward->setIcon(Icon("go-down"));
#ifdef KTIKZ_USE_KDE
	// activate completion
	KCompletion *completion = ui.comboBoxFind->completionObject();
	connect(ui.comboBoxFind, SIGNAL(returnPressed(QString)), completion, SLOT(addItem(QString)));
	completion = ui.comboBoxReplace->completionObject();
	connect(ui.comboBoxReplace, SIGNAL(returnPressed(QString)), completion, SLOT(addItem(QString)));
#endif

	setFocusProxy(ui.comboBoxFind);

	connect(ui.pushButtonBackward, SIGNAL(clicked()), this, SLOT(setBackward()));
	connect(ui.pushButtonForward, SIGNAL(clicked()), this, SLOT(setForward()));
	connect(ui.pushButtonFind, SIGNAL(clicked()), this, SLOT(doFind()));
	connect(ui.pushButtonReplace, SIGNAL(clicked()), this, SLOT(doReplace()));
	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(hide()));
}

ReplaceWidget::~ReplaceWidget()
{
}

void ReplaceWidget::setBackward()
{
	ui.pushButtonBackward->setChecked(true);
	ui.pushButtonForward->setChecked(false);
}

void ReplaceWidget::setForward()
{
	ui.pushButtonBackward->setChecked(false);
	ui.pushButtonForward->setChecked(true);
}

void ReplaceWidget::setForward(bool forward)
{
	if (forward)
		setForward();
	else
		setBackward();
}

void ReplaceWidget::hide()
{
	setVisible(false);
	emit focusEditor();
}

void ReplaceWidget::doFind()
{
	const QString currentText = ui.comboBoxFind->currentText();
	if (currentText.isEmpty())
		return;
	if (ui.comboBoxFind->findText(currentText) < 0)
		ui.comboBoxFind->addItem(currentText);

	QTextDocument::FindFlags flags = 0;
	if (ui.checkBoxCaseSensitive->isChecked())
		flags |= QTextDocument::FindCaseSensitively;
	if (ui.checkBoxWholeWords->isChecked())
		flags |= QTextDocument::FindWholeWords;
	if (!ui.pushButtonForward->isChecked())
		flags |= QTextDocument::FindBackward;
	emit search(currentText, flags);
}

void ReplaceWidget::doReplace()
{
	const QString currentText = ui.comboBoxFind->currentText();
	if (currentText.isEmpty())
		return;
	const QString replacementText = ui.comboBoxReplace->currentText();
	if (ui.comboBoxFind->findText(currentText) < 0)
		ui.comboBoxFind->addItem(currentText);
	if (ui.comboBoxReplace->findText(replacementText) < 0)
		ui.comboBoxReplace->addItem(replacementText);

	QTextDocument::FindFlags flags = 0;
	if (ui.checkBoxCaseSensitive->isChecked())
		flags |= QTextDocument::FindCaseSensitively;
	if (ui.checkBoxWholeWords->isChecked())
		flags |= QTextDocument::FindWholeWords;
	if (!ui.pushButtonForward->isChecked())
		flags |= QTextDocument::FindBackward;
	emit replace(currentText, replacementText, flags);
}

void ReplaceWidget::setText(const QString &text)
{
	ui.comboBoxFind->lineEdit()->setText(text);
	ui.comboBoxFind->setFocus();
	ui.comboBoxFind->lineEdit()->selectAll();
}

void ReplaceWidget::showEvent(QShowEvent *event)
{
	ui.comboBoxFind->setFocus();
	QWidget::showEvent(event);
}

void ReplaceWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
		hide();
	else if (event->key() == Qt::Key_Return)
		doFind();
	QWidget::keyPressEvent(event);
}
