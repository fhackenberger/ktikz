/***************************************************************************
 *   Copyright (C) 2009, 2010, 2011 by Glad Deschrijver                    *
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

#include "ktikzapplication.h"

#ifdef KTIKZ_USE_KDE
#include <KCmdLineArgs>
#include <KUrl>

#include "mainwindow.h"

KtikzApplication::KtikzApplication(int &argc, char **argv)
	: KApplication()
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);
}

void KtikzApplication::init()
{
	if (isSessionRestored())
	{
		kRestoreMainWindows<MainWindow>();
		return;
	}

	MainWindow *mainWindow = new MainWindow;
	mainWindow->show();

	KUrl url;
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (args->count() > 0)
	{
		url = args->url(0);
		if (url.isValid() && url.isLocalFile())
			mainWindow->loadUrl(url);
	}
	args->clear();
}

QString KtikzApplication::applicationName()
{
	return "KtikZ";
}
#else
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtGui/QMessageBox>
#include <QtGui/QSessionManager>
#include <QtCore/QSettings>

#include "mainwindow.h"

KtikzApplication::KtikzApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
}

void KtikzApplication::init()
{
	if (isSessionRestored())
	{
		QSettings settings(ORGNAME, APPNAME);
		settings.beginGroup("Session" + sessionId());
		const int size = settings.beginReadArray("MainWindowList");
		for (int i = 0; i < size; ++i)
		{
			settings.setArrayIndex(i);
			const QString fileName = settings.value("CurrentFile").toString();
			MainWindow *mainWindow = new MainWindow;
			mainWindow->show();
			if (!fileName.isEmpty())
				mainWindow->loadUrl(QUrl(fileName));
		}
		settings.endArray();
		settings.remove("");
		settings.endGroup();

		return;
	}

	MainWindow *mainWindow = new MainWindow;
	mainWindow->show();

	QStringList args = arguments();
	if (args.size() > 1)
		mainWindow->loadUrl(QUrl(QFileInfo(args.at(1)).absoluteFilePath()));
}

QString KtikzApplication::applicationName()
{
	return "QtikZ";
}

void KtikzApplication::commitData(QSessionManager &manager)
{
	if (manager.allowsInteraction())
	{
		QList<int> saveDocuments;
		QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
		// find out which documents to save
		for (int i = 0; i < mainWindowList.size(); ++i)
		{
			if (mainWindowList.at(i)->isDocumentModified())
			{
				const int ret = QMessageBox::warning(mainWindowList.at(i),
				                                     applicationName(),
				                                     tr("The document \"%1\" has been modified.\n"
				                                        "Do you want to save your changes?").arg(mainWindowList.at(i)->url().fileName()),
				                                     QMessageBox::Save | QMessageBox::Default,
				                                     QMessageBox::Discard,
				                                     QMessageBox::Cancel | QMessageBox::Escape);
				if (ret == QMessageBox::Save)
					saveDocuments << i; // store the number of the document that has to be saved
				else if (ret == QMessageBox::Cancel)
					manager.cancel();
			}
		}
		manager.release(); // release the manager so that it can handle other programs
		// do the actual saving (after releasing the manager, so the manager doesn't have to wait until the saving has finished)
		for (int i = 0; i < saveDocuments.size(); ++i)
		{
			if (!mainWindowList.at(saveDocuments.at(i))->save())
				manager.cancel();
		}
	}
	else
	{
// TODO save unsaved documents to a temporary file
	}
}

void KtikzApplication::saveState(QSessionManager &manager)
{
	QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
	if (mainWindowList.size() == 0)
		return;

	// in X11 the session manager calls saveState() also on startup,
	// we don't want to save anything at startup, so we define a
	// discard command (in main.cpp) which cleans up the unnecessary
	// session information
	QStringList discard = QStringList(applicationFilePath());
	discard << QLatin1String("--discard") << sessionId();
	manager.setDiscardCommand(discard); // this lets xsm hang, but it works in real session management in KDE Plasma

	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup("Session" + sessionId());
	settings.beginWriteArray("MainWindowList");
	for (int i = 0; i < mainWindowList.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("CurrentFile", mainWindowList.at(i)->url().path());
	}
	settings.endArray();
	settings.endGroup();
}
#endif

KtikzApplication::~KtikzApplication()
{
	// don't delete the MainWindow's here because they are already deleted on
	// close, however:
	// the MainWindow's are not deleted by the session manager when the session
	// closes, therefore we explicitely delete them here so that the temporary
	// files generated by them get deleted, note that we cannot delete them
	// in commitData() or saveState() because both need access to them and it
	// is not specified which is executed first, furthermore I see no other
	// way of deleting them when KTIKZ_USE_KDE is defined
	QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
	while (!mainWindowList.isEmpty())
		delete mainWindowList.takeFirst();
}
