/***************************************************************************
 *   Copyright (C) 2007, 2011, 2012, 2014 by Glad Deschrijver              *
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

#include "editindentwidget.h"

#include <QtGui/QKeyEvent>
#include <QtCore/QSettings>

#include "../common/utils/icon.h"

IndentWidget::IndentWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    ui.pushButtonClose->setIcon(Icon(QLatin1String("dialog-cancel")));

    readSettings();

    connect(ui.pushButtonIndent, &QAbstractButton::clicked, this, [this]() { indent(); });
    connect(ui.pushButtonClose, &QAbstractButton::clicked, this, &IndentWidget::hide);
}

IndentWidget::~IndentWidget() { }

void IndentWidget::readSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("Editor"));
    ui.radioButtonSpaces->setChecked(
            settings.value(QLatin1String("Indent/InsertChar"), '\t').toChar() == QLatin1Char(' '));
    ui.radioButtonTabs->setChecked(settings.value(QLatin1String("Indent/InsertChar"), '\t').toChar()
                                   != QLatin1Char(' '));
    ui.spinBoxSpaces->setValue(settings.value(QLatin1String("Indent/NumberOfSpaces"), 2).toInt());
    ui.spinBoxTabs->setValue(settings.value(QLatin1String("Indent/NumberOfTabs"), 1).toInt());
    settings.endGroup();
}

void IndentWidget::writeSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("Editor"));
    settings.setValue(QLatin1String("Indent/InsertChar"),
                      ui.radioButtonSpaces->isChecked() ? QVariant::fromValue(' ')
                                                        : QVariant::fromValue('\t'));
    settings.setValue(QLatin1String("Indent/NumberOfSpaces"), ui.spinBoxSpaces->value());
    settings.setValue(QLatin1String("Indent/NumberOfTabs"), ui.spinBoxTabs->value());
    settings.endGroup();
}

void IndentWidget::setUnindenting(bool isUnindenting)
{
    m_isUnindenting = isUnindenting;
    setWindowTitle(m_isUnindenting ? tr("Unindent") : tr("Indent"));
    ui.pushButtonIndent->setText(m_isUnindenting ? tr("Unin&dent") : tr("In&dent"));
}

QChar IndentWidget::insertChar() const
{
    return ui.radioButtonSpaces->isChecked() ? QLatin1Char(' ') : QLatin1Char('\t');
}

int IndentWidget::numOfInserts() const
{
    return ui.radioButtonSpaces->isChecked() ? ui.spinBoxSpaces->value() : ui.spinBoxTabs->value();
}

void IndentWidget::indent()
{
    const QChar insertChar =
            ui.radioButtonSpaces->isChecked() ? QLatin1Char(' ') : QLatin1Char('\t');
    const int numOfInserts =
            ui.radioButtonSpaces->isChecked() ? ui.spinBoxSpaces->value() : ui.spinBoxTabs->value();
    Q_EMIT indent(insertChar, numOfInserts, m_isUnindenting);
    writeSettings();
}

void IndentWidget::hide()
{
    setVisible(false);
    Q_EMIT hidden();
    readSettings();
}

void IndentWidget::showEvent(QShowEvent *event)
{
    if (ui.radioButtonSpaces->isChecked())
        setFocusProxy(ui.radioButtonSpaces);
    else
        setFocusProxy(ui.radioButtonTabs);
    QWidget::showEvent(event);
}

void IndentWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        hide();
    else if (event->key() == Qt::Key_Return)
        indent();
    QWidget::keyPressEvent(event);
}
