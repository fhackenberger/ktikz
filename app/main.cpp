/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010 by Glad Deschrijver              *
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
#include <QWidget> // needed for abort() below

#include "ktikzapplication.h"

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
#ifdef KTIKZ_USE_KDE
	const QString locale = KGlobal::locale()->language();
#else
	const QString locale = QString(QLocale::system().name());
#endif
	const QString localeShort = locale.left(2).toLower();

	bool foundTranslator = false;
	QTranslator *translator = new QTranslator(0);

	// find in transDir
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + '_' + locale, transDir);
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + '_' + localeShort, transDir);
	// find in dir which was set during compilation
#ifdef KTIKZ_TRANSLATIONS_INSTALL_DIR
	const QDir qmPath(KTIKZ_TRANSLATIONS_INSTALL_DIR);
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + '_' + locale, qmPath.absolutePath());
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + '_' + localeShort, qmPath.absolutePath());
#endif // KTIKZ_TRANSLATIONS_INSTALL_DIR
	// find in working dir
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + '_' + locale, "");
	if (!foundTranslator)
		foundTranslator = findTranslator(translator, transName + '_' + localeShort, "");

	return translator;
}

int main(int argc, char **argv)
{
	qInstallMsgHandler(debugOutput);

#ifdef KTIKZ_USE_KDE
	Q_INIT_RESOURCE(ktikz);

	KAboutData aboutData("ktikz", "ktikz", ki18n("KTikZ"), APPVERSION);
	aboutData.setShortDescription(ki18n("A TikZ Editor"));
	aboutData.setLicense(KAboutData::License_GPL_V2);
	aboutData.setCopyrightStatement(ki18n("Copyright 2007-2010 Florian Hackenberger, Glad Deschrijver"));
	aboutData.setOtherText(ki18n("This is a program for creating TikZ (from the LaTeX pgf package) diagrams."));
	aboutData.setBugAddress("florian@hackenberger.at");
	aboutData.addAuthor(ki18n("Florian Hackenberger"), ki18n("Maintainer"), "florian@hackenberger.at");
	aboutData.addAuthor(ki18n("Glad Deschrijver"), ki18n("Developer"), "glad.deschrijver@gmail.com");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KCmdLineOptions options;
	options.add("+[URL]", ki18n("TikZ document to open"));
	KCmdLineArgs::addCmdLineOptions(options);
#else
	Q_INIT_RESOURCE(qtikz);
#endif

	KtikzApplication app(argc, argv);
	QCoreApplication::setOrganizationName(ORGNAME);

#ifndef KTIKZ_USE_KDE
	QCoreApplication::setApplicationName(APPNAME);
	QCoreApplication::setApplicationVersion(APPVERSION);
#endif

	const QString translationsDirPath = qgetenv("KTIKZ_TRANSLATIONS_DIR");
	QTranslator *qtTranslator = createTranslator("qt", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	QTranslator *qtikzTranslator = createTranslator("qtikz", translationsDirPath);
	app.installTranslator(qtTranslator);
	app.installTranslator(qtikzTranslator);

	app.init();
	int success = app.exec();

	delete qtTranslator;
	delete qtikzTranslator;
	return success;
}
