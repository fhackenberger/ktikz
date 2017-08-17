/***************************************************************************
 *   Copyright (C) 2007 by Florian Hackenberger                            *
 *     <florian@hackenberger.at>                                           *
 *   Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012 by Glad Deschrijver  *
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
#include <KLocalizedString>
#else
#include <QSettings>
#endif

#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QWidget> // needed for abort() below
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "../common/utils/url.h"
#include "ktikzapplication.h"

// add copyright notice to the *.ts files; this string is not used anywhere else
static struct { const char *source; const char *comment; } copyrightString = QT_TRANSLATE_NOOP3("__Copyright__",
	"The original English text is copyrighted by the authors of the source "
	"files where the strings come from. This file is distributed under the "
	"same license as the KtikZ package. The translations in this file are "
	"copyrighted as follows.",
	"Translators: don't translate this, but put in the \"translation\" "
	"a copyright notice of the form \"This file was translated by <NAME>. "
	"Copyright (C) <YEAR> <NAME>.\" in which you fill in the year(s) of "
	"translation and your name.");

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
static void debugOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// qDebug() and qWarning() only show messages when in debug mode
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type)
	{
		case QtDebugMsg:
		case QtWarningMsg:
#ifndef QT_NO_DEBUG
			fprintf(stderr, "%s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
#endif
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			abort();
	}
}
#else
static void debugOutput(QtMsgType type, const char *msg)
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
#endif

static bool findTranslator(QTranslator *translator, const QString &transName, const QString &transDir)
{
	const QString qmFile = transName + QLatin1String(".qm");
	const QFileInfo fi(QDir(transDir), qmFile);
	if (fi.exists())
		return translator->load(qmFile, transDir);
	return false;
}

static void createTranslator(QTranslator *translator, const QString &transName, const QString &transDir)
{
	const QString locale = QString(QLocale::system().name());
	const QString localeShort = locale.left(2).toLower();

	const QStringList transDirs = QStringList() << transDir
#ifdef KTIKZ_TRANSLATIONS_INSTALL_DIR
	    << QDir(QLatin1String(KTIKZ_TRANSLATIONS_INSTALL_DIR)).absolutePath() // set during compilation
#endif // KTIKZ_TRANSLATIONS_INSTALL_DIR
	    << QString(); // working dir

	for (int i = 0; i < transDirs.size(); ++i)
	{
		if (findTranslator(translator, transName + QLatin1Char('_') + locale, transDirs.at(i)))
			return;
		if (findTranslator(translator, transName + QLatin1Char('_') + localeShort, transDirs.at(i)))
			return;
	}
}

int main(int argc, char **argv)
{
//QTime t = QTime::currentTime();
	Q_UNUSED(copyrightString);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	qInstallMessageHandler(debugOutput);
#else
	qInstallMsgHandler(debugOutput);
#endif

#ifndef KTIKZ_USE_KDE
	// discard session (X11 calls QApplication::saveState() also when the app
	// has been started, but then also calls QSessionManager::discardCommand()
	// which we define as below in order to remove unnecessary session
	// information)
	if (argc == 3 && !strcmp(argv[1], "--discard"))
	{
		QSettings settings(QString::fromLocal8Bit(ORGNAME), QString::fromLocal8Bit(APPNAME));
		settings.remove(QLatin1String("Session") + QLatin1String(argv[2])); // argv[2] contains the session id
		settings.sync();
		return 0;
	}
#endif

#ifdef KTIKZ_USE_KDE
	Q_INIT_RESOURCE(ktikz);
#else
	Q_INIT_RESOURCE(qtikz);
#endif

	KtikzApplication app(argc, argv); // slow

#ifdef KTIKZ_USE_KDE
	Q_INIT_RESOURCE(ktikz);

	KAboutData aboutData(QStringLiteral("ktikz"),i18n("KtikZ"), APPVERSION);
	aboutData.setShortDescription(i18n("A TikZ Editor"));
	aboutData.setLicense(KAboutLicense::GPL_V2);
	aboutData.setCopyrightStatement(i18n("Copyright 2007-2017 Florian Hackenberger, Glad Deschrijver, Joao Carreira"));
	aboutData.setOtherText(i18n("This is a program for creating TikZ (from the LaTeX pgf package) diagrams."));
	aboutData.setBugAddress("jfmcarreira@gmail.com");
	aboutData.addAuthor(i18n("Florian Hackenberger"), i18n("Maintainer"), "florian@hackenberger.at");
	aboutData.addAuthor(i18n("Glad Deschrijver"), i18n("Developer"), "glad.deschrijver@gmail.com");
  aboutData.addAuthor(i18n("Joao Carreira"), i18n("KDE Frameworks port"), "jfmcarreira@gmail.com");

	QCommandLineParser parser;
	aboutData.setupCommandLine(&parser);
	KAboutData::setApplicationData(aboutData);
	parser.addVersionOption();
	parser.addHelpOption();
	parser.addPositionalArgument(QLatin1String("[URL]"), i18n("TikZ document to open"));
	parser.process(app); // PORTING SCRIPT: move this to after any parser.addOption
	aboutData.processCommandLine(&parser);
#endif


	QCoreApplication::setOrganizationName(QString::fromLocal8Bit(ORGNAME));

#ifndef KTIKZ_USE_KDE
	QCoreApplication::setApplicationName(QString::fromLocal8Bit(APPNAME));
	QCoreApplication::setApplicationVersion(QString::fromLocal8Bit(APPVERSION));
#endif

	const QString translationsDirPath = QString::fromLocal8Bit(qgetenv("KTIKZ_TRANSLATIONS_DIR").constData());
	QTranslator qtTranslator;
	QTranslator qtikzTranslator;
	createTranslator(&qtTranslator, QLatin1String("qt"), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	createTranslator(&qtikzTranslator, QLatin1String("qtikz"), translationsDirPath);
	app.installTranslator(&qtTranslator);
	app.installTranslator(&qtikzTranslator);

	app.init();
//qCritical() << t.msecsTo(QTime::currentTime());
	return app.exec();
}
