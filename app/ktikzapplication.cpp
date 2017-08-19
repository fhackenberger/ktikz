/***************************************************************************
 *   Copyright (C) 2009, 2010, 2011, 2012 by Glad Deschrijver              *
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

#include "mainwindow.h"
#include "utils/url.h"

#include <QFileInfo>
#include <QUrl>
#include <QSettings>
#include <QSessionManager>
#include <QMessageBox>

KtikzApplication::KtikzApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
}

void KtikzApplication::init()
{
	if (isSessionRestored())
	{
		QSettings settings;
		settings.beginGroup(QLatin1String("Session") + sessionId());
		const int size = settings.beginReadArray(QLatin1String("MainWindowList"));
		for (int i = 0; i < size; ++i)
		{
			settings.setArrayIndex(i);
			const QString fileName = settings.value(QLatin1String("CurrentFile")).toString();
			const int lineNumber = settings.value(QLatin1String("LineNumber"), 1).toInt();
			MainWindow *mainWindow = new MainWindow;
			mainWindow->show();
			if (!fileName.isEmpty())
			{
				mainWindow->loadUrl(Url(fileName));
				mainWindow->setLineNumber(lineNumber);
			}
		}
		settings.endArray();
		settings.remove(QString());
		settings.endGroup();

		return;
	}

	MainWindow *mainWindow = new MainWindow;
	mainWindow->show();

	QStringList args = arguments();
	for (int i = 1; i < args.size(); ++i)
		mainWindow->loadUrl(Url(QFileInfo(args.at(i)).absoluteFilePath()));
}

#ifdef KTIKZ_USE_KDE
QString KtikzApplication::applicationName()
{
	return QStringLiteral("KtikZ");
}
#else
QString KtikzApplication::applicationName()
{
	return QStringLiteral("QtikZ");
}
#endif

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
				const QMessageBox::StandardButton ret = QMessageBox::warning(mainWindowList.at(i),
				                                     applicationName(),
				                                     tr("The document \"%1\" has been modified.\n"
				                                        "Do you want to save your changes?").arg(mainWindowList.at(i)->url().fileName()),
				                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
				                                     QMessageBox::Save);
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

	QSettings settings;
	settings.beginGroup(QLatin1String("Session") + sessionId());
	settings.beginWriteArray(QLatin1String("MainWindowList"));
	for (int i = 0; i < mainWindowList.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue(QLatin1String("CurrentFile"), mainWindowList.at(i)->url().path());
		settings.setValue(QLatin1String("LineNumber"), mainWindowList.at(i)->lineNumber());
	}
	settings.endArray();
	settings.endGroup();
}

KtikzApplication::~KtikzApplication()
{
	// don't delete the MainWindows here because they are already deleted on
	// close, however:
	// the MainWindows are not deleted by the session manager when the session
	// closes, therefore we explicitly delete them here so that the temporary
	// files generated by them get deleted, note that we cannot delete them
	// in commitData() or saveState() because both need access to them and it
	// is not specified which is executed first, furthermore I see no other
	// way of deleting them when KTIKZ_USE_KDE is defined
	QList<MainWindow*> mainWindowList = MainWindow::mainWindowList();
	while (!mainWindowList.isEmpty())
		delete mainWindowList.takeFirst();
}
