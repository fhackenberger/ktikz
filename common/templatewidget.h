/***************************************************************************
 *   Copyright (C) 2008, 2010, 2011, 2014                                  *
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

#ifndef TEMPLATEWIDGET_H
#define TEMPLATEWIDGET_H

#include "ui_templatewidget.h"

class UrlCompletion;

class TemplateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateWidget(QWidget *parent = 0);
    ~TemplateWidget();
    QWidget *lastTabOrderWidget();
    void setFileName(const QString &fileName);
    void setReplaceText(const QString &replace);
    void setEditor(const QString &editor);
    QString fileName() const;

Q_SIGNALS:
    void fileNameChanged(const QString &fileName);
    void focusEditor();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private Q_SLOTS:
    void selectTemplateFile();
    void editTemplateFile();
    void reloadTemplateFile();

private:
    void readRecentTemplates();
    void saveRecentTemplates();

    Ui::TemplateWidget ui;
    UrlCompletion *m_urlCompletion;

    QString m_editor;
};

#endif
