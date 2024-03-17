/***************************************************************************
 *   Copyright (C) 2008, 2009, 2013, 2014 by Glad Deschrijver              *
 *     <glad.deschrijver@gmail.com>                                        *
 *   Copyright (C) 2016 by G. Prudhomme                                    *
 *     <gprud@users.noreply.github.com>                                    * * This program is free
 *software; you can redistribute it and/or modify  * it under the terms of the GNU General Public
 *License as published by  * the Free Software Foundation; either version 2 of the License, or     *
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

#ifndef KTIKZ_CONFIGEDITORWIDGET_H
#define KTIKZ_CONFIGEDITORWIDGET_H

#include "ui_configeditorwidget.h"

class QTextCodec;
class QComboBox;

class ConfigEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigEditorWidget(QWidget *parent = 0);

    static QVariant defaultSetting(const QString &key);

    void readSettings(const QString &settingsGroup);
    void writeSettings(const QString &settingsGroup);

    void initializeEncoding();

protected:
    Ui::ConfigEditorWidget ui;

private Q_SLOTS:
    void selectFont();

    void on_encodingComboBox_currentIndexChanged(int index);

private:
    QFont m_generalFont;

    static QString codecNameToString(const QByteArray &codecName);
    static QString codecNameToString(QTextCodec *codec);
    static void fillCodecComboBox(QComboBox *cb);
    static void selectEncoding(QComboBox *cb, const QVariant &codecName);
};

#endif
