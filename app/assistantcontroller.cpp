/***************************************************************************
 *   Copyright (C) 2010 by Glad Deschrijver                                *
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

#include "assistantcontroller.h"

#include <QByteArray>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QProcess>

#include "ktikzapplication.h"

AssistantController::AssistantController()
    : m_process(0)
{
}

AssistantController::~AssistantController()
{
	if (m_process && m_process->state() == QProcess::Running)
	{
		m_process->terminate();
		m_process->waitForFinished(3000);
	}
	delete m_process;
}

bool AssistantController::startAssistant()
{
	if (!m_process)
		m_process = new QProcess();

	if (m_process->state() != QProcess::Running)
	{
		QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
		app += QLatin1String("assistant");
#else
		app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

		const QString ktikzDocFile = QLatin1String(KTIKZ_DOCUMENTATION_INSTALL_DIR) + QLatin1String("/qtikz.qhc");
		if (!QFileInfo(ktikzDocFile).isReadable())
		{
			QMessageBox::critical(0, KtikzApplication::applicationName(),
			    QObject::tr("Unable to open the help file (%1)").arg(ktikzDocFile));
			return false;
		}

		QStringList args;
		args << QLatin1String("-collectionFile")
		    << ktikzDocFile
		    << QLatin1String("-enableRemoteControl");

		m_process->start(app, args);

		if (!m_process->waitForStarted())
		{
			QMessageBox::critical(0, KtikzApplication::applicationName(),
			    QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
			return false;
		}
	}
	return true;
}

void AssistantController::showDocumentation(const QString &page)
{
	if (!startAssistant())
		return;

	if (!page.isEmpty())
	{
		QByteArray ba;
		ba.append("setSource qthelp://hackenberger.qtikz/doc/");
		m_process->write(ba + page.toLocal8Bit() + '\0');
	}
}
