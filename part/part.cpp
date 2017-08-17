/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012 by Glad Deschrijver        *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   Document watcher and reloader code copied from Okular KPart code      *
 *     which is copyrighted as follows:                                    *
 *     Copyright (C) 2002 by Wilco Greven <greven@kde.org>                 *
 *     Copyright (C) 2002 by Chris Cheney <ccheney@cheney.cx>              *
 *     Copyright (C) 2002 by Malcolm Hunter <malcolm.hunter@gmx.co.uk>     *
 *     Copyright (C) 2003-2004 by Christophe Devriese                      *
 *                           <Christophe.Devriese@student.kuleuven.ac.be>  *
 *     Copyright (C) 2003 by Daniel Molkentin <molkentin@kde.org>          *
 *     Copyright (C) 2003 by Andy Goossens <andygoossens@telenet.be>       *
 *     Copyright (C) 2003 by Dirk Mueller <mueller@kde.org>                *
 *     Copyright (C) 2003 by Laurent Montel <montel@kde.org>               *
 *     Copyright (C) 2004 by Dominique Devriese <devriese@kde.org>         *
 *     Copyright (C) 2004 by Christoph Cullmann <crossfire@babylon2k.de>   *
 *     Copyright (C) 2004 by Henrique Pinto <stampede@coltec.ufmg.br>      *
 *     Copyright (C) 2004 by Waldo Bastian <bastian@kde.org>               *
 *     Copyright (C) 2004-2008 by Albert Astals Cid <aacid@kde.org>        *
 *     Copyright (C) 2004 by Antti Markus <antti.markus@starman.ee>        *
 *     licensed under GPL v2 or later                                      *
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

#include "part.h"

#include <KPluginFactory>
#include <KAboutApplicationDialog>
#include <KAboutData>
#include <QAction>
#include <KActionCollection>
#include <KDirWatch>
#include <KMessageBox>
#include <KIO/Job>
#include <KIO/JobUiDelegate>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>
#include <QFileDialog>

#include "configdialog.h"
#include "settings.h"
#include "../common/templatewidget.h"
#include "../common/tikzpreview.h"
#include "../common/tikzpreviewcontroller.h"
#include "../common/utils/action.h"
#include "browserextension.h"

namespace KtikZ
{

Part::Part(QWidget *parentWidget, QObject *parent, const QVariantList &args)
	: KParts::ReadOnlyPart(parent)
{
	Q_UNUSED(args);

	// dirty hack: make sure that the "Export" menu and the "Template" widget are translated
	QTranslator *translator = createTranslator("qtikz");
	qApp->installTranslator(translator);

  setComponentData(KAboutData("ktikzpart", "KtikZ", APPVERSION));
// 	setComponentData(ktikzPartFactory::componentData()); // make sure that the actions of this kpart go in a separate section labeled "KtikZ Viewer" (as defined in K_EXPORT_PLUGIN above) in the "Configure Shortcuts" dialog

	m_configDialog = 0;

	Action::setActionCollection(actionCollection());
	m_tikzPreviewController = new TikzPreviewController(this);

	QWidget *mainWidget = new QWidget(parentWidget);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	mainLayout->addWidget(m_tikzPreviewController->templateWidget());
	mainLayout->addWidget(m_tikzPreviewController->tikzPreview());
	mainWidget->setLayout(mainLayout);
	setWidget(mainWidget);

	createActions();

	// document watcher and reloader
	m_watcher = new KDirWatch(this);
	connect(m_watcher, SIGNAL(dirty(QString)), this, SLOT(slotFileDirty(QString)));
	m_dirtyHandler = new QTimer(this);
	m_dirtyHandler->setSingleShot(true);
	connect(m_dirtyHandler, SIGNAL(timeout()), this, SLOT(slotDoFileDirty()));

	new BrowserExtension(this, m_tikzPreviewController); // needed to be able to use Konqueror's "Print" action

	setXMLFile("ktikzpart/ktikzpart.rc");

	applySettings();
}

Part::~Part()
{
	delete m_tikzPreviewController;
}

QWidget *Part::widget()
{
	return KParts::ReadOnlyPart::widget();
}

KAboutData *Part::createAboutData()
{
	KAboutData *aboutData = new KAboutData(QStringLiteral("ktikz"),i18n("KtikZ"), APPVERSION);
	aboutData->setShortDescription(i18n("A TikZ Viewer"));
	aboutData->setLicense(KAboutLicense::GPL_V2);
	aboutData->setCopyrightStatement(i18n("Copyright 2007-2014 Florian Hackenberger, Glad Deschrijver"));
	aboutData->setOtherText(i18n("This is a plugin for viewing TikZ (from the LaTeX pgf package) diagrams."));
	aboutData->setBugAddress("florian@hackenberger.at");
	aboutData->addAuthor(i18n("Florian Hackenberger"), i18n("Maintainer"), "florian@hackenberger.at");
	aboutData->addAuthor(i18n("Glad Deschrijver"), i18n("Developer"), "glad.deschrijver@gmail.com");
	return aboutData;
}

void Part::showAboutDialog()
{
	KAboutApplicationDialog dlg(*(createAboutData()), widget());
	dlg.exec();
}

void Part::createActions()
{
	// File
	m_saveAsAction = actionCollection()->addAction(KStandardAction::SaveAs, this, SLOT(saveAs()));
	m_saveAsAction->setWhatsThis(i18nc("@info:whatsthis", "<para>Save the document under a new name.</para>"));

	// Reload: we rely on Konqueror's "Reload" action instead of defining our own

	// Configure
	QAction *action = KStandardAction::preferences(this, SLOT(configure()), actionCollection());
	action->setText(i18nc("@action", "Configure KtikZ Viewer..."));

	// Help
	action = actionCollection()->addAction("help_about_ktikz");
	action->setText(i18n("About KtikZ Viewer"));
	action->setIcon(QIcon::fromTheme("ktikz"));
	connect(action, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
}

/***************************************************************************/

bool Part::openFile()
{
	const QString fileName = localFilePath();

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		KMessageBox::error(widget(), i18nc("@info", "Cannot read file <filename>%1</filename>:<nl/><message>%2</message>", fileName, file.errorString()), i18nc("@title:window", "File Read Error"));
		return false;
	}

	QTextStream in(&file);
	m_tikzCode = in.readAll();
	m_tikzPreviewController->generatePreview();

	// set the file to the fileWatcher
	if (url().isLocalFile())
	{
		if (!m_watcher->contains(localFilePath()))
			m_watcher->addFile(localFilePath());
		QFileInfo fi(localFilePath());
		if (!m_watcher->contains(fi.absolutePath()))
			m_watcher->addDir(fi.absolutePath());
	}
	m_fileWasRemoved = false;

	return true;
}

void Part::saveAs()
{
	const Url srcUrl = url();

	QMimeDatabase db;
	const QMimeType mimeType = db.mimeTypeForName("text/x-pgf");
	const QString tikzFilter = (mimeType.isValid()) ?
	                           mimeType.globPatterns().join(" ") + '|' + mimeType.comment()
	                           : "*.pgf *.tikz *.tex|" + i18nc("@item:inlistbox filter", "TikZ files");
	const QUrl dstUrl = QFileDialog::getSaveFileUrl(widget(), i18nc("@title:window", "Save TikZ Source File As"),
	                    srcUrl, tikzFilter + "\n*|" + i18nc("@item:inlistbox filter", "All files"));

	if (!dstUrl.isValid())
		return;

	KIO::Job *job = KIO::file_copy(srcUrl, dstUrl, -1, KIO::Overwrite | KIO::HideProgressInfo);
	connect(job, SIGNAL(result(KJob*)), this, SLOT(showJobError(KJob*)));
}

bool Part::closeUrl()
{
	if (url().isLocalFile())
	{
		m_watcher->removeFile(localFilePath());
		QFileInfo fi(localFilePath());
		m_watcher->removeDir(fi.absolutePath());
	}

	emit setWindowCaption("");
	m_fileWasRemoved = false;

	return KParts::ReadOnlyPart::closeUrl();
}

void Part::showJobError(KJob *job)
{
	if (job->error() != 0)
	{
		KIO::JobUiDelegate *ui = static_cast<KIO::JobUiDelegate*>(static_cast<KIO::Job*>(job)->ui());
		if (!ui)
		{
			qCritical() << "Saving failed; job->ui() is null.";
			return;
		}
		ui->setWindow(widget());
		ui->showErrorMessage();
	}
}

QString Part::tikzCode() const
{
	return m_tikzCode;
}

Url KtikZ::Part::url() const
{
	return Url(KParts::ReadOnlyPart::url());
}

/***************************************************************************/

void Part::slotFileDirty(const QString &path)
{
	// The beauty of this is that each start cancels the previous one.
	// This means that timeout() is only fired when there have
	// no changes to the file for the last 750 milisecs.
	// This ensures that we don't update on every other byte that gets
	// written to the file.
	if (path == localFilePath())
	{
		m_dirtyHandler->start(750);
	}
	else
	{
		QFileInfo fi(localFilePath());
		if (fi.absolutePath() == path)
		{
			// Our parent has been dirtified
			if (!QFile::exists(localFilePath()))
			{
				m_fileWasRemoved = true;
			}
			else if (m_fileWasRemoved && QFile::exists(localFilePath()))
			{
				// we need to watch the new file
				m_watcher->removeFile(localFilePath());
				m_watcher->addFile(localFilePath());
				m_dirtyHandler->start(750);
			}
		}
	}
}

void Part::slotDoFileDirty()
{
	m_tikzPreviewController->tikzPreview()->showErrorMessage(i18nc("@info:status", "Reloading the document..."));

	// close and (try to) reopen the document
	if (!KParts::ReadOnlyPart::openUrl(url()))
	{
		// start watching the file again (since we dropped it on close)
		m_watcher->addFile(localFilePath());
		m_dirtyHandler->start(750);
	}
}

/***************************************************************************/

void Part::applySettings()
{
	m_tikzPreviewController->applySettings();

	// Watch File
	QSettings settings(ORGNAME, APPNAME);
	bool watchFile = settings.value("WatchFile", true).toBool();
	if (watchFile && m_watcher->isStopped())
		m_watcher->startScan();
	if (!watchFile && !m_watcher->isStopped())
	{
		m_dirtyHandler->stop();
		m_watcher->stopScan();
	}
}

void Part::configure()
{
	if (m_configDialog == 0)
	{
		m_configDialog = new PartConfigDialog(widget());
		connect(m_configDialog, SIGNAL(settingsChanged(QString)), this, SLOT(applySettings()));
	}
	m_configDialog->readSettings();
	m_configDialog->show();
}

/***************************************************************************/
/* The following are only used to translate the "Export" menu and "Template" widget */

bool Part::findTranslator(QTranslator *translator, const QString &transName, const QString &transDir)
{
	const QString qmFile = transName + ".qm";
	if (QFileInfo(QDir(transDir), qmFile).exists())
		return translator->load(qmFile, transDir);
	return false;
}

QTranslator *Part::createTranslator(const QString &transName)
{
	const QString locale = QLocale::languageToString(QLocale().language());
	const QString localeShort = locale.left(2).toLower();

	QTranslator *translator = new QTranslator(0);
#ifdef KTIKZ_TRANSLATIONS_INSTALL_DIR
	const QDir qmPath(KTIKZ_TRANSLATIONS_INSTALL_DIR);
	bool foundTranslator = findTranslator(translator, transName + '_' + locale, qmPath.absolutePath());
	if (!foundTranslator)
		findTranslator(translator, transName + '_' + localeShort, qmPath.absolutePath());
#endif
	return translator;
}

K_PLUGIN_FACTORY(ktikzPartFactory, registerPlugin<KtikZ::Part>();)

#include "part.moc"

} // namespace KtikZ
