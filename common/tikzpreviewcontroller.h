/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014                      *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
 *   Copyright (C) 2016 by G. Prudhomme                                    *
 *     <gprud@users.noreply.github.com>                                    *
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

#ifndef KTIKZ_TIKZPREVIEWCONTROLLER_H
#define KTIKZ_TIKZPREVIEWCONTROLLER_H

#include <QtCore/QObject>
#include "tikzpreviewgenerator.h"
#include "utils/url.h"

#ifndef KTIKZ_USE_KDE
class QAction;
class QMenu;
class QToolBar;
#endif

class QPrinter;
class QTimer;
class TextCodecProfile;
class MainWidget;
class TemplateWidget;
class TikzPreview;
class Action;
class TempDir;
class ToggleAction;

class TikzPreviewController : public QObject
{
    Q_OBJECT

public:
    explicit TikzPreviewController(MainWidget *mainWidget);
    ~TikzPreviewController();

    const TextCodecProfile *textCodecProfile() const;
    const QString tempDir() const;
    const QString tempDirLocation() const;
    TemplateWidget *templateWidget() const;
    TikzPreview *tikzPreview() const;
#ifndef KTIKZ_USE_KDE
    QAction *exportAction();
    QAction *printPreviewAction();
    QAction *printAction();
    QMenu *menu();
    QList<QToolBar *> toolBars();
    void setToolBarStyle(const Qt::ToolButtonStyle &style);
#endif
    QString tikzCode() const;
    QString getLogText();
    void emptyPreview();
    void applySettings();

public Q_SLOTS:
    void printPreviewImage();
    void printImage();
    void generatePreview();
    void regeneratePreviewAfterDelay();

private Q_SLOTS:
    void setTemplateFileAndRegenerate(const QString &path);
    void setReplaceTextAndRegenerate(const QString &replace);
    void regeneratePreview();
    void abortProcess();
    void exportImage();
    void printImage(QPrinter *printer);
    void setExportActionsEnabled(bool enabled);
    void setProcessRunning(bool isRunning);
    void toggleShellEscaping(bool useShellEscaping);

Q_SIGNALS:
    void updateLog(const QString &logText, bool runFailed);
    void appendLog(const QString &logText, bool runFailed);
    void showMouseCoordinates(qreal x, qreal y, int precisionX, int precisionY);

private:
    const QString tempFileBaseName() const;
    void createActions();
    void generatePreview(TikzPreviewGenerator::TemplateStatus templateStatus);
    bool setTemplateFile(const QString &path);
    Url getExportUrl(const Url &url, const QString &mimeType) const;

    MainWidget *m_mainWidget;
    QWidget *m_parentWidget;

    TemplateWidget *m_templateWidget;
    TikzPreview *m_tikzPreview;
    TikzPreviewGenerator *m_tikzPreviewGenerator;

    QTimer *m_regenerateTimer;

#ifndef KTIKZ_USE_KDE
    QList<QToolBar *> m_toolBars;
#endif
    Action *m_exportAction;
    Action *m_printPreviewAction;
    Action *m_printAction;
    Action *m_procStopAction;
    ToggleAction *m_shellEscapeAction;

    TempDir *m_tempDir;
    QString m_currentFileName;
};

#endif
