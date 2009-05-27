/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
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

#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
	QLabel *pixmapLabel = new QLabel;
	pixmapLabel->setPixmap(QPixmap(":/images/ktikz-128.png"));
	QLabel *label = new QLabel(QString("<h1>%1 %2</h1><p>%3</p><p>%4</p>")
	    .arg(tr("TikZ editor"))
	    .arg(APPVERSION)
	    .arg(tr("Copyright 2007-2009 Florian Hackenberger and Glad Deschrijver"))
	    .arg(tr("This is a program for creating TikZ (from the LaTeX pgf package) diagrams.")));
	label->setWordWrap(true);

	QWidget *topWidget = new QWidget;
	QHBoxLayout *topLayout = new QHBoxLayout;
	topLayout->addWidget(pixmapLabel);
	topLayout->addWidget(label);
	topWidget->setLayout(topLayout);

	QTextEdit *textEdit = new QTextEdit("<p>"
	    + tr("This program is free software; you can redistribute it and/or "
	    "modify it under the terms of the GNU General Public License "
	    "as published by the Free Software Foundation; either version 2 "
	    "of the License, or (at your option) any later version.")
	    + "</p><p>"
	    + tr("This program is distributed in the hope that it will be useful, "
	    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
	    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
	    "GNU General Public License for more details.")
	    + "</p>");
	textEdit->setReadOnly(true);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(topWidget);
	mainLayout->addWidget(textEdit);
	mainLayout->addWidget(buttonBox);
	mainLayout->setSpacing(10);
	buttonBox->setFocus();

	setWindowTitle(tr("About TikZ editor"));
}
