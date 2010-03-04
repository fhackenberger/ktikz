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

#include "filedialog.h"

#ifdef KTIKZ_USE_KDE
#include <KMessageBox>
#include <KIO/NetAccess>

Url FileDialog::getOpenUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	KFileDialog openDialog(dir, filter, parent, 0);
	openDialog.setOperationMode(KFileDialog::Opening);
	openDialog.setCaption(caption);
	if (!openDialog.exec())
		return Url();

	const KUrl openUrl = openDialog.selectedUrl();
	if (!openUrl.isValid() || openUrl.isEmpty())
		return Url();
	return openUrl;
}

Url FileDialog::getSaveUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	KFileDialog saveAsDialog(dir, filter, parent, 0);
	saveAsDialog.setOperationMode(KFileDialog::Saving);
	saveAsDialog.setCaption(caption);
	if (!saveAsDialog.exec())
		return Url();

	const KUrl saveAsUrl = saveAsDialog.selectedUrl();
	if (!saveAsUrl.isValid() || saveAsUrl.isEmpty())
		return Url();
	if (KIO::NetAccess::exists(saveAsUrl, KIO::NetAccess::DestinationSide, parent))
	{
		if (KMessageBox::warningContinueCancel(parent,
//		    i18nc("@info", "A file named <filename>%1</filename> already exists. "
//		    "Are you sure you want to overwrite it?", saveAsUrl.fileName()), QString(),
//		    KGuiItem(i18nc("@action:button", "Overwrite"))) != KMessageBox::Continue)
		    tr("File \"%1\" already exists.\nDo you want to overwrite it?").arg(saveAsUrl.fileName()), QString(),
		    KGuiItem(tr("Overwrite", "@action:button"))) != KMessageBox::Continue)
			return Url();
	}
	return saveAsUrl;
}
#else
#include <QCoreApplication>
#include <QMessageBox>

Url FileDialog::getOpenUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	const QStringList filterList = filter.split('\n');
	QString parsedFilter;
	for (int i = 0; i < filterList.size(); ++i)
	{
		const QStringList filterItems = filterList.at(i).split('|');
		if (i > 0)
			parsedFilter += ";;";
		parsedFilter += filterItems.at(1) + " (" + filterItems.at(0) + ")";
	}

	const QString openFileName = QFileDialog::getOpenFileName(parent, caption, dir.path(), parsedFilter);
	if (openFileName.isEmpty())
		return Url();
	return Url(openFileName);
}

Url FileDialog::getSaveUrl(QWidget *parent, const QString &caption, const Url &dir, const QString &filter)
{
	const QStringList filterList = filter.split('\n');
	QString parsedFilter;
	for (int i = 0; i < filterList.size(); ++i)
	{
		const QStringList filterItems = filterList.at(i).split('|');
		if (i > 0)
			parsedFilter += ";;";
		parsedFilter += filterItems.at(1) + " (" + filterItems.at(0) + ")";
	}

	const QString saveAsFileName = QFileDialog::getSaveFileName(parent, caption, dir.path(), parsedFilter);
	if (saveAsFileName.isEmpty())
		return Url();

	if (QFile::exists(saveAsFileName))
	{
		if (QMessageBox::warning(parent, QCoreApplication::applicationName(),
		    tr("File \"%1\" already exists.\nDo you want to overwrite it?").arg(saveAsFileName),
		    QMessageBox::Save | QMessageBox::Default,
		    QMessageBox::Discard | QMessageBox::Escape) == QMessageBox::Discard)
			return Url();
	}
	return Url(saveAsFileName);
}
#endif
