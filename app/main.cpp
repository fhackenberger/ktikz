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

//#include <QDebug>
#ifdef KTIKZ_USE_KDE
#include <KAboutData>
#include <KCmdLineArgs>
#include <KUrl>
#else
#include <QtCore/QSettings>
#endif
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QWidget> // needed for abort() below
#else
#include <QtGui/QWidget> // needed for abort() below
#endif

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
#ifdef KTIKZ_USE_KDE
	const QString locale = KGlobal::locale()->language();
#else
	const QString locale = QString(QLocale::system().name());
#endif
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

	KAboutData aboutData("ktikz", "ktikz", ki18n("KtikZ"), APPVERSION);
	aboutData.setShortDescription(ki18n("A TikZ Editor"));
	aboutData.setLicense(KAboutData::License_GPL_V2);
	aboutData.setCopyrightStatement(ki18n("Copyright 2007-2014 Florian Hackenberger, Glad Deschrijver"));
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

	KtikzApplication app(argc, argv); // slow
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
