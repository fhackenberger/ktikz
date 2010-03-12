/***************************************************************************
 *   Copyright (C) 2008-2010 by Glad Deschrijver                           *
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

#include "part.h"

#include <KAboutData>
#include <KAction>
#include <KActionCollection>
#include <KFileDialog>
#include <KMessageBox>
#include <KIO/Job>
//#include <KIO/JobUiDelegate>
#include <KIO/NetAccess>
#include <KParts/GenericFactory>

#include "configdialog.h"
#include "settings.h"
#include "../common/templatewidget.h"
#include "../common/tikzpreview.h"
#include "../common/tikzpreviewcontroller.h"
#include "../common/utils/action.h"

typedef KParts::GenericFactory<Part> KTikZPartFactory;
K_EXPORT_COMPONENT_FACTORY(ktikzpart, KTikZPartFactory)

Part::Part(QWidget *parentWidget, QObject *parent, const QStringList &args)
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

KAboutData* Part::createAboutData()
{
	KAboutData *aboutData = new KAboutData("ktikzpart", "ktikz",
	    ki18n("KTikZ KPart"), APPVERSION);
	aboutData->setShortDescription(ki18n("A TikZ Viewer"));
	aboutData->setLicense(KAboutData::License_GPL_V3);
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

//	setStatusBarText(i18nc("@info:status", "File loaded"));
	return true;
}

void Part::saveAs()
{
	const KUrl srcUrl = url();

	const KMimeType::Ptr mimeType = KMimeType::mimeType("text/x-pgf");
	const QString tikzFilter = (mimeType) ?
	    mimeType->patterns().join(" ") + "|" + mimeType->comment()
	    : "*.pgf *.tikz *.tex|" + i18nc("@item:inlistbox filter", "TikZ files");
	KFileDialog saveAsDialog(srcUrl, tikzFilter + "\n*|" + i18nc("@item:inlistbox filter", "All files"), widget());
	saveAsDialog.setOperationMode(KFileDialog::Saving);
	saveAsDialog.setCaption(i18nc("@title:window", "Save TikZ Source File As"));
	saveAsDialog.setSelection(srcUrl.fileName());
	if (!saveAsDialog.exec())
		return;
	const KUrl dstUrl = saveAsDialog.selectedUrl();
	if (!dstUrl.isValid())
		return;

	if (KIO::NetAccess::exists(dstUrl, KIO::NetAccess::DestinationSide, widget()))
	{
		if (KMessageBox::warningContinueCancel(widget(),
		    i18nc("@info", "A file named <filename>%1</filename> already exists. "
		    "Are you sure you want to overwrite it?", dstUrl.fileName()), QString(),
		    KGuiItem(i18nc("@action:button", "Overwrite"))) != KMessageBox::Continue)
			return;
	}

	KIO::Job *job = KIO::file_copy(srcUrl, dstUrl, -1, KIO::Overwrite | KIO::HideProgressInfo);
	connect(job, SIGNAL(result(KJob*)), m_tikzPreviewController, SLOT(showJobError(KJob*)));
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

QString Part::tikzCode() const
{
	return m_tikzCode;
}

Url Part::url() const
{
	return KParts::ReadOnlyPart::url();
}

/***************************************************************************/

void Part::applySettings()
{
	m_tikzPreviewController->applySettings();
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
