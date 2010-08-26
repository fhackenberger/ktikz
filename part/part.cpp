/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Glad Deschrijver                    *
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

#include <KAboutData>
#include <KAction>
#include <KActionCollection>
#include <KDirWatch>
#include <KFileDialog>
#include <KMessageBox>
#include <KIO/Job>
//#include <KIO/JobUiDelegate>
#include <KIO/NetAccess>
#include <KParts/GenericFactory>
#include <QSettings>
#include <QTimer>

#include "configdialog.h"
#include "settings.h"
#include "../common/templatewidget.h"
#include "../common/tikzpreview.h"
#include "../common/tikzpreviewcontroller.h"
#include "../common/utils/action.h"

K_PLUGIN_FACTORY(ktikzPartFactory, registerPlugin<Part>();)
K_EXPORT_PLUGIN(ktikzPartFactory("ktikz","ktikz") )

Part::Part(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    : KParts::ReadOnlyPart(parent)
{
	Q_UNUSED(args);

	m_configDialog = 0;

//	setObjectName("ktikz#");

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
	connect(m_watcher, SIGNAL(dirty(const QString&)), this, SLOT(slotFileDirty(const QString&)));
	m_dirtyHandler = new QTimer(this);
	m_dirtyHandler->setSingleShot(true);
	connect(m_dirtyHandler, SIGNAL(timeout()),this, SLOT(slotDoFileDirty()));

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
	KAboutData *aboutData = new KAboutData("ktikzpart", "ktikz",
	    ki18n("KTikZ KPart"), APPVERSION);
	aboutData->setShortDescription(ki18n("A TikZ Viewer"));
	aboutData->setLicense(KAboutData::License_GPL_V2);
	aboutData->setCopyrightStatement(ki18n("Copyright 2007-2010 Florian Hackenberger, Glad Deschrijver"));
	aboutData->setOtherText(ki18n("This is a plugin for viewing TikZ (from the LaTeX pgf package) diagrams."));
	aboutData->setBugAddress("florian@hackenberger.at");
	aboutData->addAuthor(ki18n("Florian Hackenberger"), ki18n("Maintainer"), "florian@hackenberger.at");
	aboutData->addAuthor(ki18n("Glad Deschrijver"), ki18n("Developer"), "glad.deschrijver@gmail.com");
	return aboutData;
}

void Part::createActions()
{
	// File
	m_saveAsAction = actionCollection()->addAction(KStandardAction::SaveAs, this, SLOT(saveAs()));
	m_saveAsAction->setWhatsThis(i18nc("@info:whatsthis", "<para>Save the document under a new name.</para>"));

/*
	KAction *reloadAction = actionCollection()->add<KAction>("file_reload");
	reloadAction->setText(i18nc("@action", "Reloa&d"));
	reloadAction->setIcon(KIcon("view-refresh"));
	reloadAction->setWhatsThis(i18nc("@info:whatsthis", "Reload the current document from disk."));
	connect(reloadAction, SIGNAL(triggered()), this, SLOT(slotReload()));
	reloadAction->setShortcut(KStandardShortcut::reload());
	m_reloadAction = reloadAction;
*/

	// Configure
	KAction *action = KStandardAction::preferences(this, SLOT(configure()), actionCollection());
	action->setText(i18nc("@action", "Configure KTikZ Viewer..."));
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

//	setStatusBarText(i18nc("@info:status", "File loaded"));
	return true;
}

void Part::saveAs()
{
	const KUrl srcUrl = url();

	const KMimeType::Ptr mimeType = KMimeType::mimeType("text/x-pgf");
	const QString tikzFilter = (mimeType) ?
	    mimeType->patterns().join(" ") + '|' + mimeType->comment()
	    : "*.pgf *.tikz *.tex|" + i18nc("@item:inlistbox filter", "TikZ files");
	const KUrl dstUrl = KFileDialog::getSaveUrl(srcUrl,
	    tikzFilter + "\n*|" + i18nc("@item:inlistbox filter", "All files"),
	    widget(), i18nc("@title:window", "Save TikZ Source File As"),
	    KFileDialog::ConfirmOverwrite);
	if (!dstUrl.isValid())
		return;

	KIO::Job *job = KIO::file_copy(srcUrl, dstUrl, -1, KIO::Overwrite | KIO::HideProgressInfo);
	connect(job, SIGNAL(result(KJob*)), m_tikzPreviewController, SLOT(showJobError(KJob*)));
}

bool Part::closeUrl()
{
	if (url().isLocalFile())
	{
		m_watcher->removeFile(localFilePath());
		QFileInfo fi(localFilePath());
		m_watcher->removeDir(fi.absolutePath());
	}
	m_fileWasRemoved = false;

	return KParts::ReadOnlyPart::closeUrl();
}

/*
void Part::showJobError(KJob *job)
{
	if (job->error() != 0)
	{
		KIO::JobUiDelegate *ui = static_cast<KIO::Job*>(job)->ui();
		if (!ui)
		{
			kError() << "Saving failed; job->ui() is null.";
			return;
		}
		ui->setWindow(widget());
		ui->showErrorMessage();
	}
}
*/

/*
void Part::slotReload()
{
	// stop the dirty handler timer, otherwise we may conflict with the
	// auto-refresh system
	m_dirtyHandler->stop();

	slotDoFileDirty();
}
*/

QString Part::tikzCode() const
{
	return m_tikzCode;
}

Url Part::url() const
{
	return KParts::ReadOnlyPart::url();
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
