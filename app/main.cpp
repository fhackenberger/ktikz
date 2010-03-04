/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *   Copyright (C) 2007-2009 by Glad Deschrijver                           *
 *   florian@hackenberger.at                                               *
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

#ifdef KTIKZ_USE_KDE
#include <KAboutData>
#include <KCmdLineArgs>
#include <KUrl>
#endif
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include "ktikzapplication.h"
#include "mainwindow.h"

void debugOutput(QtMsgType type, const char *msg)
{
	// qDebug() and qWarning() only show messages when in debug mode
	switch (type)
	{
		case QtDebugMsg:
		case QtWarningMsg:
#ifndef QT_NO_DEBUG
			fprintf(stderr, "%s\n", msg);
#endif
			break;
		case QtCriticalMsg:
			fprintf(stderr, "%s\n", msg);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", msg);
			abort();
	}
}

bool findTranslator(QTranslator *translator, const QString &transName, const QString &transDir)
{
	const QString qmFile = transName + ".qm";
	const QFileInfo fi(QDir(transDir), qmFile);
	if (fi.exists())
		return translator->load(qmFile, transDir);
	return false;
}

QTranslator *createTranslator(const QString &transName, const QString &transDir)
{
	const QString locale = QString(QLocale::system().name());
	const QString localeShort = locale.left(2).toLower();

	bool foundTranslator = false;
	QTranslator *translator = new QTranslator(0);

	// find in transDir
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + "_" + locale, transDir);
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + "_" + localeShort, transDir);
	// find in dir which was set during compilation
#ifdef KTIKZ_INSTALL_TRANSLATIONS
	const QDir qmPath(KTIKZ_INSTALL_TRANSLATIONS);
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + "_" + locale, qmPath.absolutePath());
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + "_" + localeShort, qmPath.absolutePath());
#endif // KTIKZ_INSTALL_TRANSLATIONS
	// find in working dir
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + "_" + locale, "");
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + "_" + localeShort, "");

	return translator;
}

int main(int argc, char **argv)
{
	Q_INIT_RESOURCE(application);
	qInstallMsgHandler(debugOutput);

#ifdef KTIKZ_USE_KDE
	KAboutData aboutData("ktikz", 0, ki18n("KTikZ"), APPVERSION);
	KCmdLineArgs::init(argc, argv, &aboutData);

	KCmdLineOptions options;
	options.add("+[URL]", ki18n("TikZ document to open"));
	KCmdLineArgs::addCmdLineOptions(options);
#endif

	KtikzApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Florian Hackenberger");
	QCoreApplication::setApplicationName("TikZ editor");
	QCoreApplication::setApplicationVersion(APPVERSION);

	const QString translationsDirPath = qgetenv("KTIKZ_TRANSLATIONS_DIR");
	app.installTranslator(createTranslator("qt", QLibraryInfo::location(QLibraryInfo::TranslationsPath)));
	app.installTranslator(createTranslator("ktikz", translationsDirPath));

	app.init();

	int success = app.exec();
	return success;
}
