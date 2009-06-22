/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#include "ktikzapplication.h"

#ifdef KTIKZ_USE_KDE

#else
#include <QFileInfo>
#endif
#include <QMessageBox>
#include <QSessionManager>
#include <QSettings>

#include "mainwindow.h"

#include <poppler-qt4.h>

KtikzApplication::KtikzApplication(int argc, char **argv)
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
	m_saveSession = false;
}

void KtikzApplication::init()
{
	if (isSessionRestored())
	{
		QSettings settings;
		settings.beginGroup("Session" + qApp->sessionId());
		const int size = settings.beginReadArray("MainWindowList");
		for (int i = 0; i < size; ++i)
		{
			settings.setArrayIndex(i);
			const QString fileName = settings.value("CurrentFile").toString();
			MainWindow *mainWindow = new MainWindow;
			mainWindow->show();
			if (!fileName.isEmpty())
				mainWindow->loadFile(fileName);
		}
		settings.endArray();
		settings.remove("");
		settings.endGroup();

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
//			mainWindow->loadUrl(url);
			mainWindow->loadFile(url.path());
	}
	args->clear();
#else
	if (m_args.size() > 1)
	{
		const QFileInfo fi(m_args.at(1));
		mainWindow->loadFile(fi.absoluteFilePath());
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
	return "KTikZ";
}

void KtikzApplication::commitData(QSessionManager &manager)
{
	m_saveSession = true;

	if (manager.allowsInteraction())
	{
		QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
		for (int i = 0; i < mainWindowList.size(); ++i)
		{
			if (mainWindowList.at(i)->isDocumentModified())
			{
				const int ret = QMessageBox::warning(mainWindowList.at(i),
				    applicationName(),
				    tr("The document \"%1\" has been modified.\n"
				       "Do you want to save your changes?").arg(mainWindowList.at(i)->currentFileName()),
				    QMessageBox::Save | QMessageBox::Default,
				    QMessageBox::Discard,
			    	QMessageBox::Cancel | QMessageBox::Escape);
				if (ret == QMessageBox::Save)
				{
					manager.release();
					if (!mainWindowList.at(i)->save())
						manager.cancel();
				}
				else if (ret == QMessageBox::Cancel)
					manager.cancel();
			}
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

	if (!m_saveSession)
		return;

	QSettings settings;
	settings.beginGroup("Session" + qApp->sessionId());
	settings.beginWriteArray("MainWindowList");
	QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
	for (int i = 0; i < mainWindowList.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("CurrentFile", mainWindowList.at(i)->currentFileFullPath());
	}
	settings.endArray();
	settings.endGroup();
}
