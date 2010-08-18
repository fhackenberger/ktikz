/***************************************************************************
 *   Copyright (C) 2009, 2010 by Glad Deschrijver                          *
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
#else
#include <QFileInfo>
#include <QUrl>
#include <QMessageBox>
#include <QSessionManager>
#include <QSettings>
#endif

#include "mainwindow.h"

KtikzApplication::KtikzApplication(int &argc, char **argv)
#ifdef KTIKZ_USE_KDE
    : KApplication()
#else
    : QApplication(argc, argv)
#endif
{
#ifdef KTIKZ_USE_KDE
	Q_UNUSED(argc);
	Q_UNUSED(argv);
#else
	for (int i = 0; i < argc; ++i)
		m_args << QString::fromLocal8Bit(argv[i]);
#endif
	m_firstTime = true;
}

void KtikzApplication::init()
{
	if (isSessionRestored())
	{
#ifdef KTIKZ_USE_KDE
		kRestoreMainWindows<MainWindow>();
#else
		QSettings settings(ORGNAME, APPNAME);
		settings.beginGroup("Session" + qApp->sessionId());
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

		m_firstTime = false;
#endif
		return;
	}

	MainWindow *mainWindow = new MainWindow;
	mainWindow->show();

#ifdef KTIKZ_USE_KDE
	KUrl url;
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (args->count() > 0)
	{
		url = args->url(0);
		if (url.isValid() && url.isLocalFile())
			mainWindow->loadUrl(url);
	}
	args->clear();
#else
	if (m_args.size() > 1)
	{
		const QFileInfo fi(m_args.at(1));
		mainWindow->loadUrl(QUrl(fi.absoluteFilePath()));
	}
	m_args.clear();
#endif
}

KtikzApplication::~KtikzApplication()
{
	// don't delete the MainWindow's here because they are already deleted on close
}

QString KtikzApplication::applicationName()
{
#ifdef KTIKZ_USE_KDE
	return "KTikZ";
#else
	return "QTikZ";
#endif
}

#ifndef KTIKZ_USE_KDE
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
		// do the actual saving
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
	Q_UNUSED(manager);

	QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
	if (mainWindowList.size() == 0)
		return;

#ifdef Q_WS_X11
	// in X11 the session manager calls savedState also on startup,
    // we don't want to save anything at startup, so we return
	// this is a dirty hack: it would be better to actually determine
	// *when* this function is called
	if (m_firstTime)
	{
		m_firstTime = false;
		return;
	}
#endif

	QSettings settings(ORGNAME, APPNAME);
	settings.beginGroup("Session" + qApp->sessionId());
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
