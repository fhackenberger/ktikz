/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010, 2012 by Glad Deschrijver        *
 *     <glad.deschrijver@gmail.com>                                        *
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

// #include <QDebug>
#include "mainwindow.h"

#ifdef KTIKZ_USE_KDE
#  include <KActionCollection>
#  include <KConfigGroup>
#  include <KMessageBox>
#  include <KStandardAction>
#  include <KXMLGUIFactory>
#else
#  include <QLocale>
#  include "aboutdialog.h"
#  include "assistantcontroller.h"
#endif

#include <QMenuBar>
#include <QScreen>
#include <QStatusBar>
#include <QtCore5Compat/QTextCodec>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWhatsThis>

#include "configdialog.h"
#include "ktikzapplication.h"
#include "logtextedit.h"
#include "tikzcommandinserter.h"
#include "tikzdocumentationcontroller.h"
#include "tikzeditorviewabstract.h"
#include "tikzeditorhighlighter.h"
#include "tikzeditorview.h"
#include "usercommandinserter.h"
#include "../common/templatewidget.h"
#include "../common/tikzpreviewcontroller.h"
#include "../common/tikzpreview.h"
#include "../common/utils/action.h"
#include "../common/utils/file.h"
#include "../common/utils/filedialog.h"
#include "../common/utils/icon.h"
#include "../common/utils/recentfilesaction.h"
#include "../common/utils/standardaction.h"
#include "../common/utils/toggleaction.h"
#include "../common/utils/url.h"

#ifdef KTIKZ_USE_KTEXTEDITOR
#  include "tikzktexteditorview.h"
#endif

QList<MainWindow *> MainWindow::s_mainWindowList;

MainWindow::MainWindow()
{
// QTime t = QTime::currentTime();
#ifndef KTIKZ_USE_KDE
    m_aboutDialog = 0;
    m_assistantController = 0;
#endif
    m_configDialog = 0;
    m_isModifiedExternally = false;
    m_insertAction = 0;

    s_mainWindowList.append(this);

#ifndef KTIKZ_USE_KDE
    QStringList themeSearchPaths;
    themeSearchPaths << QDir::homePath() + QLatin1String("/.local/share/icons/");
    themeSearchPaths << QIcon::themeSearchPaths();
    QIcon::setThemeSearchPaths(themeSearchPaths);
#endif

    setAttribute(Qt::WA_DeleteOnClose);
#ifdef KTIKZ_USE_KDE
    setObjectName(QLatin1String("ktikz#"));
    setWindowIcon(QIcon::fromTheme(QLatin1String("ktikz")));
    Action::setActionCollection(actionCollection());
#else
    setObjectName(QLatin1String("qtikz#") + QString::number(s_mainWindowList.size()));
    setWindowIcon(QIcon(QLatin1String(":/icons/qtikz-22.png")));
#endif

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // qCritical() << t.msecsTo(QTime::currentTime());
    m_tikzPreviewController = new TikzPreviewController(this);
    // qCritical() << "TikzPreviewController" << t.msecsTo(QTime::currentTime());

    m_useKTextEditor = false;
#ifdef KTIKZ_USE_KTEXTEDITOR
    QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
    m_useKTextEditor = settings.value(QLatin1String("EditorWidget"), 0).toInt() == 1;
    if (m_useKTextEditor) {
        m_tikzEditorView = m_tikzKTextEditor = new TikzKTextEditorView(this);
    } else
#endif
    {
        m_tikzEditorView = m_tikzQtEditorView = new TikzEditorView(this);
        m_tikzHighlighter = new TikzHighlighter(m_tikzQtEditorView->editor()->document());
    }
    // qCritical() << "TikzEditorView" << t.msecsTo(QTime::currentTime());
    m_commandInserter = new TikzCommandInserter(this);
    m_userCommandInserter = new UserCommandInserter(this);

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_tikzPreviewController->templateWidget());
    mainLayout->addWidget(m_tikzEditorView);

    m_logDock = new QDockWidget(this);
    m_logDock->setObjectName(QLatin1String("LogDock"));
    m_logDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_logDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable
                           | QDockWidget::DockWidgetFloatable);
    m_logDock->setWindowTitle(tr("&Messages"));
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
    m_logTextEdit = new LogTextEdit;
    m_logTextEdit->setWhatsThis(tr("<p>The messages produced by "
                                   "LaTeX are shown here.  If your TikZ code contains errors, "
                                   "then a red border will appear and the errors will be "
                                   "highlighted.</p>"));
    m_logDock->setWidget(m_logTextEdit);

    m_previewDock = new QDockWidget(this);
    m_previewDock->setObjectName(QLatin1String("PreviewDock"));
    m_previewDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_previewDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable
                               | QDockWidget::DockWidgetFloatable);
    m_previewDock->setWindowTitle(tr("Previe&w"));
    m_previewDock->setWidget(m_tikzPreviewController->tikzPreview());
    addDockWidget(Qt::RightDockWidgetArea, m_previewDock);

    setCentralWidget(mainWidget);

    createActions();
#ifndef KTIKZ_USE_KDE
    createToolBars(); // run first in order to be able to add file/editToolBar->toggleViewAction()
                      // to the menu
    createMenus();
#endif
    // qCritical() << "createMenus" << t.msecsTo(QTime::currentTime());
    createCommandInsertWidget(); // must happen after createMenus and before readSettings
    createStatusBar();

#ifdef KTIKZ_USE_KDE
    setupGUI(ToolBar | Keys | StatusBar | Save);
#  ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#  endif
    {
        createGUI("ktikzui.rc");
    }
    guiFactory()->addClient(this);
#  ifdef KTIKZ_USE_KTEXTEDITOR
    if (m_useKTextEditor) {
        createGUI("ktikzui_frameworks.rc");
        guiFactory()->addClient(m_tikzKTextEditor->view());

        connect(m_tikzKTextEditor, &TikzKTextEditorView::documentUrlChanged, this,
                &MainWindow::changedUrl);
    }
#  endif
#endif

    // TODO: check this
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
    {
        setTabOrder(m_tikzPreviewController->templateWidget()->lastTabOrderWidget(),
                    m_tikzQtEditorView->editor());
        connect(m_tikzQtEditorView, &TikzEditorView::showStatusMessage, statusBar(),
                &QStatusBar::showMessage);
    }

    connect(m_commandInserter, &TikzCommandInserter::showStatusMessage, statusBar(),
            &QStatusBar::showMessage);

    connect(m_tikzEditorView, SIGNAL(modificationChanged(bool)), this,
            SLOT(setDocumentModified(bool)));
    connect(m_tikzEditorView, SIGNAL(cursorPositionChanged(int, int)), this,
            SLOT(showCursorPosition(int, int)));

    connect(m_tikzEditorView, SIGNAL(focusIn()), this, SLOT(checkForFileChanges()));
    connect(m_tikzEditorView, SIGNAL(focusOut()), this, SLOT(saveLastInternalModifiedDateTime()));

    connect(m_tikzPreviewController, &TikzPreviewController::updateLog, m_logTextEdit,
            [this](const QString &logText, bool runFailed) {
                m_logTextEdit->updateLog(logText, runFailed);
            });
    connect(m_tikzPreviewController, &TikzPreviewController::appendLog, m_logTextEdit,
            [this](const QString &logText, bool runFailed) {
                m_logTextEdit->appendLog(logText, runFailed);
            });
    connect(m_tikzPreviewController, &TikzPreviewController::showMouseCoordinates, this,
            &MainWindow::showMouseCoordinates);

    connect(m_userCommandInserter, &UserCommandInserter::updateCompleter, this,
            &MainWindow::updateCompleter);

    readSettings(); // must be run after defining tikzController and tikzHighlighter, and after
                    // creating the toolbars, and after the connects
    // qCritical() << "readSettings()" << t.msecsTo(QTime::currentTime());

    setCurrentUrl(Url());
    setDocumentModified(false);
    saveLastInternalModifiedDateTime();
    m_tikzEditorView->setFocus();

    // delayed initialization
    //	QTimer::singleShot(0, this, SLOT(init())); // this causes flicker at startup and init() is
    // not executed in a separate thread anyway :(
    init();
    // qCritical() << "mainwindow" << t.msecsTo(QTime::currentTime());
}

MainWindow::~MainWindow()
{
    s_mainWindowList.removeAll(this);

    writeSettings();

#ifndef KTIKZ_USE_KDE
    if (m_aboutDialog)
        delete m_aboutDialog;
    if (m_assistantController)
        delete m_assistantController;
#endif

    delete m_tikzPreviewController;
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
        m_tikzHighlighter->deleteLater();
}

void MainWindow::init()
{
// QTime t = QTime::currentTime();
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
        m_tikzQtEditorView->setPasteEnabled();

    TikzCommandInserter::loadCommands();
// qCritical() << "TikzCommandInserter::loadCommands()" << t.msecsTo(QTime::currentTime());
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (m_useKTextEditor)
        m_commandInserter->setEditor(m_tikzKTextEditor->editor());
    else
#endif
    {
        m_commandInserter->setEditor(m_tikzQtEditorView->editor());
        m_tikzHighlighter->setHighlightingRules(m_commandInserter->getHighlightingRules());
    }

    if (m_insertAction)
        m_insertAction->setMenu(m_commandInserter->getMenu());
    else
        m_commandInserter->showItemsInDockWidget();
    connect(m_userCommandInserter, QOverload<const QString &>::of(&UserCommandInserter::insertTag),
            m_commandInserter, [this](const QString &tag) { m_commandInserter->insertTag(tag); });

    // the following disconnect ensures that the following signal is not unnecessarily triggered
    // twice when a file is loaded in a new window
    disconnect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
               SLOT(regeneratePreviewAfterDelay()));

    applySettings(); // must do this in order to load the command completions
    // qCritical() << "applySettings()" << t.msecsTo(QTime::currentTime());

    if (m_buildAutomatically)
        connect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
                SLOT(regeneratePreviewAfterDelay()));

    if (m_tikzPreviewController->tempDir()
                .isEmpty()) // then the temporary directory could not be created
        m_logTextEdit->updateLog(tr("Error: unable to create a temporary directory in \"%1\". This "
                                    "program will not work!")
                                         .arg(m_tikzPreviewController->tempDirLocation()),
                                 true);
    // qCritical() << "init()" << t.msecsTo(QTime::currentTime());
}

QWidget *MainWindow::widget()
{
    return this;
}

#ifdef KTIKZ_USE_KDE
bool MainWindow::queryClose()
{
    return maybeSave();
}

void MainWindow::readProperties(const KConfigGroup &group)
{
    const Url url(group.readPathEntry(QLatin1String("CurrentUrl"), QString()));
    if (url.isValid() && !url.isEmpty())
        loadUrl(url);
}

void MainWindow::saveProperties(KConfigGroup &group)
{
    group.writePathEntry(QLatin1String("CurrentUrl"), m_currentUrl.url());
}
#endif

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        QApplication::restoreOverrideCursor();
        event->accept();
    } else
        event->ignore();
}

void MainWindow::newFile()
{
    MainWindow *newMainWindow = new MainWindow;
    newMainWindow->show();
}

bool MainWindow::closeFile()
{
    if (maybeSave()) {
        disconnect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
                   SLOT(regeneratePreviewAfterDelay()));
        m_tikzEditorView->clear();
        connect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
                SLOT(regeneratePreviewAfterDelay()));
        setCurrentUrl(Url());
        m_tikzPreviewController->emptyPreview(); // abort still running processes
        m_logTextEdit->updateLog(QString(), false); // clear log window
        m_mouseCoordinatesLabel->setText(QString());
        return true;
    }
    return false;
}

void MainWindow::open()
{
    const Url openUrl = FileDialog::getOpenUrl(this, tr("Open PGF source file"), m_lastUrl,
                                               QLatin1String("text/x-pgf"));
    if (openUrl.isValid() && !openUrl.isEmpty())
        loadUrl(openUrl);
}

bool MainWindow::save()
{
    if (!m_currentUrl.isValid() || m_currentUrl.isEmpty())
        return saveAs();
    else {
        if (m_isModifiedExternally) {
            checkForFileChanges(Saving);
            return true;
        } else
            return saveUrl(m_currentUrl);
    }
}

bool MainWindow::saveAs()
{
    const Url saveAsUrl = FileDialog::getSaveUrl(this, tr("Save PGF source file"),
                                                 Url(m_currentUrl), QLatin1String("text/x-pgf"));
    if (!saveAsUrl.isValid() || saveAsUrl.isEmpty())
        return false;
    return saveUrl(saveAsUrl);
}

void MainWindow::reload()
{
    const QUrl currentUrl = m_currentUrl;
    if (closeFile()) {
        saveLastInternalModifiedDateTime();
        loadUrl(currentUrl);
    }
}

void MainWindow::checkForFileChanges(const FileCheckMoment &moment)
{
    if (moment == FocusIn) {
        QDateTime lastExternalModifiedDateTime(QFileInfo(m_currentUrl.path()).lastModified());
        if (lastExternalModifiedDateTime > m_lastInternalModifiedDateTime)
            m_isModifiedExternally = true;
        else // when the fileChangedWarningMessageBox below is cancelled, the main window is focused
             // in again with m_isModifiedExternally == true so this slot is called again, but now
             // m_lastInternalModifiedDateTime has been updated (during focusOut) and now we don't
             // want to show the dialog again
            return;
    }

    if (!m_isModifiedExternally)
        return;

    m_isModifiedExternally = false;
    QPointer<QMessageBox> fileChangedWarningMessageBox = new QMessageBox(this);
    fileChangedWarningMessageBox->setText(
            tr("The document was modified by another program.\nWhat do you want to do?"));
    fileChangedWarningMessageBox->setWindowTitle(KtikzApplication::applicationName());
    fileChangedWarningMessageBox->setIcon(QMessageBox::Warning);
    QAbstractButton *overwriteButton =
            fileChangedWarningMessageBox->addButton(tr("&Overwrite"), QMessageBox::AcceptRole);
    QAbstractButton *reloadButton;
    switch (moment) {
    case Saving:
        reloadButton = fileChangedWarningMessageBox->addButton(tr("&Save under another name"),
                                                               QMessageBox::AcceptRole);
        break;
    case Closing:
        reloadButton = fileChangedWarningMessageBox->addButton(tr("&Close without saving"),
                                                               QMessageBox::AcceptRole);
        break;
    case FocusIn:
    default:
        reloadButton = fileChangedWarningMessageBox->addButton(tr("&Reload file"),
                                                               QMessageBox::AcceptRole);
        break;
    }
    fileChangedWarningMessageBox->addButton(QMessageBox::Cancel);
    fileChangedWarningMessageBox->exec();
    if (fileChangedWarningMessageBox->clickedButton() == overwriteButton)
        saveUrl(m_currentUrl);
    else if (fileChangedWarningMessageBox->clickedButton() == reloadButton)
        switch (moment) {
        case Saving:
            saveAs();
            break;
        case Closing:
            // do nothing since the file will be closed anyway
            break;
        case FocusIn:
        default:
            reload();
            break;
        }
    else // cancel (check again on "Save" or "Close")
        m_isModifiedExternally = true;
    delete fileChangedWarningMessageBox;
}

void MainWindow::saveLastInternalModifiedDateTime()
{
    m_lastInternalModifiedDateTime = QDateTime::currentDateTime();
}

/***************************************************************************/

void MainWindow::showTikzDocumentation()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    const QString tikzDocFile = TikzDocumentationController::tikzDocumentationPath();
    const bool tikzDocFileExists =
            QFileInfo(tikzDocFile).exists(); // true if tikzDocFile is local and exists
    const QUrl tikzDocUrl =
            tikzDocFileExists ? QUrl(QLatin1String("file:///") + tikzDocFile) : QUrl(tikzDocFile);

    if (!QDesktopServices::openUrl(tikzDocUrl)) {
        if (!tikzDocFileExists)
            QMessageBox::warning(
                    this, KtikzApplication::applicationName(),
                    tr("Cannot find TikZ documentation.\n"
                       "Go to Settings -> Configure %1 and change in the \"General\" tab "
                       "the path to the TikZ documentation.")
                            .arg(KtikzApplication::applicationName()));
        else // if tikzDocFile is local and exists then failure to open it is caused by not finding
             // the correct application
            QMessageBox::warning(this, KtikzApplication::applicationName(),
                                 tr("Cannot open TikZ documentation.\n"
                                    "No application is found which can open the file \"%1\".")
                                         .arg(tikzDocFile));
    }

    QApplication::restoreOverrideCursor();
}

#ifndef KTIKZ_USE_KDE
void MainWindow::about()
{
    if (!m_aboutDialog)
        m_aboutDialog = new AboutDialog(this);
    m_aboutDialog->exec();
}

void MainWindow::showDocumentation()
{
    if (!m_assistantController)
        m_assistantController = new AssistantController;
    m_assistantController->showDocumentation();
}
#endif

/***************************************************************************/

bool MainWindow::isDocumentModified() const
{
    return isWindowModified();
}

void MainWindow::setDocumentModified(bool isModified)
{
    setWindowModified(isModified);
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
    {
        m_saveAction->setEnabled(isModified);
        m_saveAsAction->setEnabled(m_currentUrl.isValid() && !m_currentUrl.isEmpty());
    }
}

void MainWindow::updateLog()
{
    m_logTextEdit->updateLog(m_tikzPreviewController->getLogText());
    //	m_logTextEdit->updateLog(m_tikzController->getLogText(), m_tikzController->hasRunFailed());
}

/***************************************************************************/

#ifdef KTIKZ_USE_KDE
void MainWindow::toggleWhatsThisMode()
{
    if (QWhatsThis::inWhatsThisMode())
        QWhatsThis::leaveWhatsThisMode();
    else
        QWhatsThis::enterWhatsThisMode();
}
#endif

void MainWindow::createActions()
{

    // Editor related actions
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
    {
        m_saveAction = StandardAction::save(this, SLOT(save()), this);
        m_saveAction->setStatusTip(tr("Save the current document to disk"));
        m_saveAction->setWhatsThis(tr("<p>Save the current document to disk.</p>"));

        m_saveAsAction = StandardAction::saveAs(this, SLOT(saveAs()), this);
        m_saveAsAction->setStatusTip(tr("Save the document under a new name"));
        m_saveAsAction->setWhatsThis(tr("<p>Save the document under a new name.</p>"));

        m_reloadAction = new Action(Icon(QStringLiteral("view-refresh")), tr("Reloa&d"), this,
                                    QLatin1String("file_reload"));
        m_reloadAction->setShortcut(QKeySequence::Refresh);
        m_reloadAction->setStatusTip(tr("Reload the current document"));
        m_reloadAction->setWhatsThis(tr("<p>Reload the current document from disk.</p>"));
        connect(m_reloadAction, &Action::triggered, this, &MainWindow::reload);
    }

    // General action
    m_newAction = StandardAction::openNew(this, SLOT(newFile()), this);
    m_newAction->setStatusTip(tr("Create a new document"));
    m_newAction->setWhatsThis(tr("<p>Create a new document.</p>"));

    m_openAction = StandardAction::open(this, SLOT(open()), this);
    m_openAction->setStatusTip(tr("Open an existing file"));
    m_openAction->setWhatsThis(tr("<p>Open an existing file.</p>"));

    m_openRecentAction = StandardAction::openRecent(this, SLOT(loadUrl(QUrl)), this);
    m_openRecentAction->setStatusTip(tr("Open a recently opened file"));
    m_openRecentAction->setWhatsThis(tr("<p>Open a recently opened file.</p>"));

    m_closeAction = StandardAction::close(this, SLOT(closeFile()), this);
    m_closeAction->setStatusTip(tr("Close the current document"));
    m_closeAction->setWhatsThis(tr("<p>Close the current document.</p>"));

    m_exitAction = StandardAction::quit(this, SLOT(close()), this);
    m_exitAction->setStatusTip(tr("Exit the application"));
    m_exitAction->setWhatsThis(tr("<p>Exit the application.</p>"));

    // View
    m_buildAction = new Action(Icon(QLatin1String("run-build")), tr("&Build"), this,
                               QLatin1String("build"));
    // m_buildAction->setShortcut(tr("Ctrl+B", "View|Build"));
    m_buildAction->setStatusTip(tr("Build preview"));
    m_buildAction->setWhatsThis(
            tr("<p>Generate preview by building the current TikZ code in the editor.</p>"));
    connect(m_buildAction, &Action::triggered, m_tikzPreviewController,
            &TikzPreviewController::regeneratePreviewAfterDelay);

    m_viewLogAction = new Action(Icon(QLatin1String("run-build-file")), tr("View &Log"), this,
                                 QLatin1String("view_log"));
    m_viewLogAction->setStatusTip(tr("View log messages produced by the last executed process"));
    m_viewLogAction->setWhatsThis(tr("<p>Show the log messages produced by the last executed "
                                     "process in the Messages box.</p>"));
    connect(m_viewLogAction, &Action::triggered, this, &MainWindow::updateLog);

    // Configure
    m_configureAction = StandardAction::preferences(this, SLOT(configure()), this);
    m_configureAction->setText(tr("&Configure %1...").arg(KtikzApplication::applicationName()));
    m_configureAction->setStatusTip(tr("Configure the settings of this application"));
    m_configureAction->setWhatsThis(tr("<p>Configure the settings of this application.</p>"));

#ifdef KTIKZ_USE_KDE
    addActionCloneToCollection(QLatin1String("toggle_preview"), m_previewDock->toggleViewAction());
    addActionCloneToCollection(QLatin1String("toggle_log"), m_logDock->toggleViewAction());
#endif

    // Help
    m_showTikzDocAction = new Action(Icon(QLatin1String("help-contents")), tr("TikZ &Manual"), this,
                                     QLatin1String("show_tikz_doc"));
    m_showTikzDocAction->setStatusTip(tr("Show the manual of TikZ and PGF"));
    m_showTikzDocAction->setWhatsThis(tr("<p>Show the manual of TikZ and PGF.</p>"));
    connect(m_showTikzDocAction, &Action::triggered, this, &MainWindow::showTikzDocumentation);

#ifdef KTIKZ_USE_KDE
    m_whatsThisAction = KStandardAction::whatsThis(this, SLOT(toggleWhatsThisMode()), this);
#else
    m_helpAction = new QAction(Icon(QLatin1String("help-contents")),
                               tr("%1 &Handbook").arg(KtikzApplication::applicationName()), this);
    m_helpAction->setStatusTip(tr("Show the application's documentation"));
    m_helpAction->setShortcut(QKeySequence::HelpContents);
    connect(m_helpAction, &QAction::triggered, this, &MainWindow::showDocumentation);

    m_whatsThisAction = QWhatsThis::createAction(this);
    m_whatsThisAction->setIcon(Icon(QLatin1String("help-contextual")));
    m_whatsThisAction->setStatusTip(tr("Show simple description of any widget"));

    m_aboutAction = new QAction(QIcon(QLatin1String(":/icons/qtikz-22.png")),
                                tr("&About %1").arg(KtikzApplication::applicationName()), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);

    m_aboutQtAction =
            new QAction(QIcon(QLatin1String(":/icons/qt-logo-22.png")), tr("About &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
#endif
}

#ifdef KTIKZ_USE_KDE
void MainWindow::addActionCloneToCollection(const QString &actionName, QAction *action)
{
    // XXX This is a dirty hack to avoid the warning "Attempt to use QAction with KXMLGUIFactory"
    KToggleAction *actionClone = new KToggleAction(this);
    actionCollection()->addAction(actionName, actionClone);
    actionClone->setText(action->text());
    actionClone->setIcon(action->icon());
    connect(action, &QAction::toggled, actionClone, &KToggleAction::setChecked);
    connect(actionClone, &KToggleAction::triggered, action, &QAction::trigger);
}
#else
void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_openRecentAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addAction(m_tikzPreviewController->exportAction());
    fileMenu->addSeparator();
    fileMenu->addAction(m_reloadAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_tikzPreviewController->printPreviewAction());
    fileMenu->addAction(m_tikzPreviewController->printAction());
    fileMenu->addSeparator();
    fileMenu->addAction(m_closeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    menuBar()->addMenu(m_tikzEditorView->editMenu());
    menuBar()->addMenu(m_tikzEditorView->bookmarksMenu());

    QMenu *viewMenu = m_tikzPreviewController->menu();
    viewMenu->insertAction(viewMenu->actions().at(viewMenu->actions().size() - 2), m_buildAction);
    viewMenu->addAction(m_viewLogAction);
    menuBar()->addMenu(viewMenu);

    m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
    QMenu *toolBarMenu = new QMenu(tr("&Toolbars"), this);
    toolBarMenu->setIcon(Icon(QLatin1String("configure-toolbars")));
    toolBarMenu->menuAction()->setStatusTip(tr("Show or hide toolbars"));
    toolBarMenu->addAction(m_fileToolBar->toggleViewAction());
    toolBarMenu->addAction(m_editToolBar->toggleViewAction());
    toolBarMenu->addAction(m_viewToolBar->toggleViewAction());
    toolBarMenu->addAction(m_runToolBar->toggleViewAction());
    m_fileToolBar->toggleViewAction()->setStatusTip(
            tr("Show toolbar \"%1\"").arg(m_fileToolBar->windowTitle()));
    m_editToolBar->toggleViewAction()->setStatusTip(
            tr("Show toolbar \"%1\"").arg(m_editToolBar->windowTitle()));
    m_viewToolBar->toggleViewAction()->setStatusTip(
            tr("Show toolbar \"%1\"").arg(m_viewToolBar->windowTitle()));
    m_runToolBar->toggleViewAction()->setStatusTip(
            tr("Show toolbar \"%1\"").arg(m_runToolBar->windowTitle()));
    m_settingsMenu->addMenu(toolBarMenu);
    m_sideBarMenu = new QMenu(tr("&Sidebars"), this);
    m_sideBarMenu->setIcon(Icon(QLatin1String("configure-toolbars")));
    m_sideBarMenu->menuAction()->setStatusTip(tr("Show or hide sidebars"));
    m_sideBarMenu->addAction(m_previewDock->toggleViewAction());
    m_sideBarMenu->addAction(m_logDock->toggleViewAction());
    m_previewDock->toggleViewAction()->setStatusTip(
            tr("Show sidebar \"%1\"").arg(m_previewDock->windowTitle()));
    m_logDock->toggleViewAction()->setStatusTip(
            tr("Show sidebar \"%1\"").arg(m_logDock->windowTitle()));
    m_settingsMenu->addMenu(m_sideBarMenu);
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction(m_configureAction);
    connect(m_fileToolBar->toggleViewAction(), &QAction::toggled, this,
            &MainWindow::setToolBarStatusTip);
    connect(m_editToolBar->toggleViewAction(), &QAction::toggled, this,
            &MainWindow::setToolBarStatusTip);
    connect(m_viewToolBar->toggleViewAction(), &QAction::toggled, this,
            &MainWindow::setToolBarStatusTip);
    connect(m_runToolBar->toggleViewAction(), &QAction::toggled, this,
            &MainWindow::setToolBarStatusTip);
    connect(m_previewDock->toggleViewAction(), &QAction::toggled, this,
            &MainWindow::setDockWidgetStatusTip);
    connect(m_logDock->toggleViewAction(), &QAction::toggled, this,
            &MainWindow::setDockWidgetStatusTip);

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_helpAction);
    helpMenu->addAction(m_showTikzDocAction);
    helpMenu->addAction(m_whatsThisAction);
    helpMenu->addSeparator();
    helpMenu->addAction(m_aboutAction);
    helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::createToolBars()
{
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->setObjectName(QLatin1String("FileToolBar"));
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);
    m_fileToolBar->addAction(m_saveAction);
    m_fileToolBar->addAction(m_closeAction);

    m_editToolBar = m_tikzEditorView->toolBar();
    addToolBar(m_editToolBar);

    QList<QToolBar *> viewAndRunToolBars = m_tikzPreviewController->toolBars();
    addToolBar(viewAndRunToolBars.at(0));
    viewAndRunToolBars.at(1)->insertAction(viewAndRunToolBars.at(1)->actions().at(0),
                                           m_buildAction);
    viewAndRunToolBars.at(1)->addAction(m_viewLogAction);
    addToolBar(viewAndRunToolBars.at(1));
    m_viewToolBar = viewAndRunToolBars.at(0);
    m_runToolBar = viewAndRunToolBars.at(1);

    setToolBarStyle();
}

void MainWindow::setToolBarStyle()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));

    int toolBarStyleNumber = settings.value(QLatin1String("ToolBarStyle"), 0).toInt();
    Qt::ToolButtonStyle toolBarStyle = Qt::ToolButtonIconOnly;
    switch (toolBarStyleNumber) {
    case 0:
        toolBarStyle = Qt::ToolButtonIconOnly;
        break;
    case 1:
        toolBarStyle = Qt::ToolButtonTextOnly;
        break;
    case 2:
        toolBarStyle = Qt::ToolButtonTextBesideIcon;
        break;
    case 3:
        toolBarStyle = Qt::ToolButtonTextUnderIcon;
        break;
    }

    m_fileToolBar->setToolButtonStyle(toolBarStyle);
    m_editToolBar->setToolButtonStyle(toolBarStyle);
    m_tikzPreviewController->setToolBarStyle(toolBarStyle);

    settings.endGroup();
}
#endif

void MainWindow::createCommandInsertWidget()
{
    // insert global commands widget
    QSettings settings;
    bool commandsInDock = settings.value(QLatin1String("CommandsInDock"), false).toBool();

    if (commandsInDock) {
        m_commandsDock = m_commandInserter->getDockWidget(this);
        addDockWidget(Qt::LeftDockWidgetArea, m_commandsDock);

#ifdef KTIKZ_USE_KDE
        actionCollection()->addAction(QLatin1String("toggle_commands_list"),
                                      m_commandsDock->toggleViewAction());
#else
        m_sideBarMenu->insertAction(m_sideBarMenu->actions().at(1),
                                    m_commandsDock->toggleViewAction());
#endif
        connect(m_commandsDock->toggleViewAction(), &QAction::toggled, this,
                &MainWindow::setDockWidgetStatusTip);
    } else {
        // add commands action (menu will be added later in init())
        m_insertAction = new Action(tr("&Insert"), this, QLatin1String("insert"));
#ifndef KTIKZ_USE_KDE
        menuBar()->insertAction(m_settingsMenu->menuAction(), m_insertAction);
#endif
    }

    // add user commands menu
#ifdef KTIKZ_USE_KDE
    QMenu *userMenu = m_userCommandInserter->getMenu();
    QAction *userInsertAction = new Action(userMenu->title(), this, QLatin1String("user_insert"));
    userInsertAction->setMenu(userMenu);
#else
    menuBar()->insertMenu(m_settingsMenu->menuAction(), m_userCommandInserter->getMenu());
#endif
}

void MainWindow::createStatusBar()
{
    QStatusBar *status = statusBar();

    m_positionLabel = new QLabel(status);
    m_positionLabel->setMinimumWidth(150);
    status->addPermanentWidget(m_positionLabel, 0);

    m_mouseCoordinatesLabel = new QLabel(status);
    m_mouseCoordinatesLabel->setMinimumWidth(300);
    status->addPermanentWidget(m_mouseCoordinatesLabel, 0);

    status->showMessage(tr("Ready"));
}

void MainWindow::setDockWidgetStatusTip(bool enabled)
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString dockName = action->text();
    dockName.remove(QLatin1Char('&'));
    if (enabled)
        action->setStatusTip(tr("Hide sidebar \"%1\"").arg(dockName));
    else
        action->setStatusTip(tr("Show sidebar \"%1\"").arg(dockName));
}

void MainWindow::setToolBarStatusTip(bool enabled)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (enabled)
        action->setStatusTip(tr("Hide toolbar \"%1\"").arg(action->text()));
    else
        action->setStatusTip(tr("Show toolbar \"%1\"").arg(action->text()));
}

/***************************************************************************/

void MainWindow::configure()
{
    if (!m_configDialog) {
        m_configDialog = new ConfigDialog(this);
        m_configDialog->setTranslatedHighlightTypeNames(
                TikzHighlighter::getTranslatedHighlightTypeNames());
        m_configDialog->setHighlightTypeNames(TikzHighlighter::getHighlightTypeNames());
        m_configDialog->setDefaultHighlightFormats(TikzHighlighter::getDefaultHighlightFormats());
        connect(m_configDialog, &ConfigDialog::settingsChanged, this, &MainWindow::applySettings);
    }
    disconnect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
               SLOT(regeneratePreviewAfterDelay()));
    m_configDialog->readSettings();
    m_configDialog->exec();
    if (m_buildAutomatically)
        connect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
                SLOT(regeneratePreviewAfterDelay()));
}

void MainWindow::applySettings()
{
    QSettings settings;

#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
        m_tikzQtEditorView->applySettings();

    m_tikzPreviewController->applySettings();

    settings.beginGroup(QLatin1String("Preview"));
    m_buildAutomatically = settings.value(QLatin1String("BuildAutomatically"), true).toBool();
    m_buildAction->setVisible(!m_buildAutomatically);
    if (!settings.value(QLatin1String("ShowCoordinates"), true).toBool())
        m_mouseCoordinatesLabel->setText(QString());
    settings.endGroup();

    settings.beginGroup(QLatin1String("Editor"));
    m_useCompletion = settings.value(QLatin1String("UseCompletion"), true).toBool();
    updateCompleter();
    settings.beginGroup(QLatin1String("encoding"));
    QVariant qv = settings.value(QLatin1String("default"));
    constexpr auto kDefaultEncoding = QStringConverter::Utf8;
    setCurrentEncoding(qv.isNull() ? kDefaultEncoding
                                   : QStringConverter::encodingForName(qv.toByteArray())
                                             .value_or(kDefaultEncoding));
    qv = settings.value(QLatin1String("encoder"));
    if (!qv.isNull()) {
        m_overrideEncoder = QStringConverter::encodingForName(qv.toByteArray());
    } else {
        m_overrideEncoder.reset();
    }
    qv = settings.value(QLatin1String("decoder"));
    if (!qv.isNull()) {
        m_overrideDecoder = QStringConverter::encodingForName(qv.toByteArray());
    } else {
        m_overrideDecoder.reset();
    }
    m_encoderBom = settings.value(QLatin1String("bom"), true).toBool();
    settings.endGroup();

#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
    {
        m_tikzHighlighter->applySettings();
        m_tikzHighlighter->rehighlight();
    }

    m_openRecentAction->createRecentFilesList();
#ifndef KTIKZ_USE_KDE
    setToolBarStyle();
#endif
}

void MainWindow::readSettings()
{
    m_openRecentAction->loadEntries();

    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    const int screenWidth = this->screen()->geometry().width();
    QSize size;
    if (screenWidth > 1200)
        size = settings.value(QLatin1String("size"), QSize(1200, 600)).toSize();
    else if (screenWidth > 1024)
        size = settings.value(QLatin1String("size"), QSize(1024, 600)).toSize();
    else
        size = settings.value(QLatin1String("size"), QSize(800, 600)).toSize();
    resize(size);
    restoreState(settings.value(QLatin1String("MainWindowState")).toByteArray());
    settings.endGroup();

    // none of the problems below seem to persist in May 2014
    // still do the following (see applySettings()) here in order to avoid flicker in the toolbar
    //	settings.beginGroup(QLatin1String("Preview"));
    //	m_buildAutomatically = settings.value(QLatin1String("BuildAutomatically"), true).toBool();
    //	m_buildAction->setVisible(!m_buildAutomatically);
    //	settings.endGroup();
    // still do the following here in order to avoid a crash when a file is opened in a new window
    //	m_openRecentAction->createRecentFilesList();
    // still do this here, otherwise the compilation fails when a file is loaded in a new window
    //	m_tikzPreviewController->applySettings();
}

void MainWindow::writeSettings()
{
    m_openRecentAction->saveEntries();

    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    settings.setValue(QLatin1String("size"), size());
    settings.setValue(QLatin1String("MainWindowState"), QMainWindow::saveState());
    settings.endGroup();
}

/***************************************************************************/

bool MainWindow::maybeSave()
{
    checkForFileChanges(Closing);
    if (m_isModifiedExternally) // if the user presses "Cancel" when asked to overwrite or close the
                                // file, then we abort the closing procedure
        return false;
    if (m_tikzEditorView->isModified()) {
        const int ret =
                QMessageBox::warning(this, KtikzApplication::applicationName(),
                                     tr("The document has been modified.\n"
                                        "Do you want to save your changes?"),
                                     QMessageBox::Save | QMessageBox::Default, QMessageBox::Discard,
                                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadUrl(const QUrl &url)
{
    //	loadUrl(Url(url));
    //}

    // void MainWindow::loadUrl(const Url &url)
    //{
    // QTime t = QTime::currentTime();
    //  check whether the file can be opened
    if (!url.isValid() || url.isEmpty())
        return;

    File file(url, File::ReadOnly);
    if (!file.open(QFile::Text)) {
        QMessageBox::warning(
                this, KtikzApplication::applicationName(),
                tr("Cannot read file \"%1\":\n%2.").arg(url.path()).arg(file.errorString()));
        m_openRecentAction->removeUrl(url);
        return;
    }

    // only open a new window (if necessary) if the file can actually be opened
    if (!m_tikzEditorView->isEmpty()) {
        MainWindow *newMainWindow = new MainWindow;
        newMainWindow->loadUrl(url);
        newMainWindow->show();
        return;
    }

    // set current url before loading its contents in m_tikzEditorView->editor()
    // because this latter action causes m_tikzPreviewController->generatePreview()
    // to be called which calls url()
    setCurrentUrl(url);

    // load the file and generate preview
    // qCritical() << "loadUrl" << t.msecsTo(QTime::currentTime());
    disconnect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
               SLOT(regeneratePreviewAfterDelay()));

#ifdef KTIKZ_USE_KTEXTEDITOR
    if (m_useKTextEditor) {
        m_tikzKTextEditor->document()->openUrl(url);
    } else
#endif
    {
        QTextStream in(file.file());
        QApplication::setOverrideCursor(Qt::WaitCursor);
        this->configureStreamDecoding(in);
        m_tikzQtEditorView->editor()->setPlainText(in.readAll());
        setCurrentEncoding(in.encoding());
    }

    QApplication::restoreOverrideCursor();
    // qCritical() << "loadUrl" << t.msecsTo(QTime::currentTime());
    m_tikzPreviewController->generatePreview();
    // qCritical() << "loadUrl" << t.msecsTo(QTime::currentTime());
    if (m_buildAutomatically)
        connect(m_tikzEditorView, SIGNAL(contentsChanged()), m_tikzPreviewController,
                SLOT(regeneratePreviewAfterDelay()));

    m_lastUrl = url;
    m_openRecentAction->addUrl(url);
    // qCritical() << "loadUrl" << t.msecsTo(QTime::currentTime());
    //	statusBar()->showMessage(tr("File loaded"), 2000); // this is slow
    //       statusBar()->showMessage(tr("File loaded using %1 codec").arg(QString(
    //       m_currentEncoding->name())), 2000);
    // qCritical() << "loadUrl" << t.msecsTo(QTime::currentTime());
}

void MainWindow::changedUrl(const QUrl &url)
{
    setCurrentUrl(url);
}

bool MainWindow::saveUrl(const QUrl &url)
{
    if (!url.isValid() || url.isEmpty())
        return false;

    File file(url, File::WriteOnly);
    if (!file.open(QFile::Text)) {
        QMessageBox::warning(
                this, KtikzApplication::applicationName(),
                tr("Cannot write file \"%1\":\n%2").arg(url.path()).arg(file.errorString()));
        return false;
    }

    QTextStream out(file.file());
    QApplication::setOverrideCursor(Qt::WaitCursor);

    this->configureStreamEncoding(out);
    out << m_tikzEditorView->text();
    out.flush();
    QApplication::restoreOverrideCursor();

    if (!file.close()) {
        QMessageBox::warning(
                this, KtikzApplication::applicationName(),
                tr("Cannot write file \"%1\":\n%2").arg(url.path()).arg(file.errorString()));
        return false;
    }

    m_lastUrl = url;
    setCurrentUrl(url);
    m_openRecentAction->addUrl(url);
    saveLastInternalModifiedDateTime(); // make sure that the save in this function is not seen as
                                        // an "external" save
    statusBar()->showMessage(tr("File saved", "@info:status"), 2000);
    return true;
}

void MainWindow::setCurrentEncoding(QStringConverter::Encoding codec)
{
    m_currentEncoding = codec;
    // TODO: implement user warning and suggestion to reload the file.
}

QUrl MainWindow::url() const
{
    return m_currentUrl;
}

void MainWindow::setCurrentUrl(const QUrl &url)
{
    m_currentUrl = url;
    m_tikzEditorView->setModified(false);
    setDocumentModified(false);
    setWindowTitle(tr("%1[*] - %2")
                           .arg(strippedName(m_currentUrl))
                           .arg(KtikzApplication::applicationName()));
}

QString MainWindow::strippedName(const QUrl &url) const
{
    if (!url.isValid() || url.isEmpty())
        return QLatin1String("untitled.txt");
    const QString fileName = url.fileName();
    return (fileName.isEmpty()) ? QLatin1String("untitled.txt") : fileName;
}

QStringConverter::Encoding MainWindow::getEncoder() const
{
    return this->m_overrideEncoder.value_or(this->m_currentEncoding);
}

void MainWindow::configureStreamEncoding(QTextStream &textStream)
{
    textStream.setEncoding(this->getEncoder());
    textStream.setGenerateByteOrderMark(this->m_encoderBom);
}

void MainWindow::configureStreamDecoding(QTextStream &textStream)
{
    if (m_overrideDecoder) {
        // textStream.setCodec(m_overrideDecoder);
    }
    textStream.setAutoDetectUnicode(true);
}

/***************************************************************************/

void MainWindow::setLineNumber(int lineNumber)
{
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (!m_useKTextEditor)
#endif
        m_tikzQtEditorView->goToLine(lineNumber - 1);
}

int MainWindow::lineNumber() const
{
#ifdef KTIKZ_USE_KTEXTEDITOR
    if (m_useKTextEditor)
        return 0; // TODO: do something
    else
#endif
        return m_tikzQtEditorView->lineNumber();
}

/***************************************************************************/

void MainWindow::showCursorPosition(int row, int col)
{
    m_positionLabel->setText(tr("Line: %1\tCol: %2", "@info:status")
                                     .arg(QString::number(row))
                                     .arg(QString::number(col)));
}

void MainWindow::showMouseCoordinates(qreal x, qreal y, int precisionX, int precisionY)
{
    m_mouseCoordinatesLabel->setText(tr("Preview: x = %1\ty = %2", "@info:status")
                                             .arg(QLocale::system().toString(x, 'f', precisionX))
                                             .arg(QLocale::system().toString(y, 'f', precisionY)));
}

/***************************************************************************/

QString MainWindow::tikzCode() const
{
    return m_tikzEditorView->text();
}

/***************************************************************************/

void MainWindow::updateCompleter()
{
    // QTime t = QTime::currentTime();
    QStringList words = m_commandInserter->getCommandWords();
    words << m_userCommandInserter->getCommandWords();
    words.sort();
    words.removeDuplicates();
    m_tikzEditorView->updateCompleter(m_useCompletion, words);
    // qCritical() << "updateCompleter" << t.msecsTo(QTime::currentTime());
}
