/***************************************************************************
 *   Copyright (C) 2008-2009 by Glad Deschrijver                           *
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

#ifndef KTIKZ_PART_H
#define KTIKZ_PART_H

#include "../common/mainwidget.h"
#include <KParts/ReadOnlyPart>

class KAboutData;
class KAction;

class PartConfigDialog;
class TikzPreviewController;

/** Provides a kpart for viewing TikZ graphics
 * @author Florian Hackenberger, Glad Deschrijver
 */
class Part : public KParts::ReadOnlyPart, public MainWidget
{
	Q_OBJECT

public:
	Part(QWidget *parentWidget, QObject *parent, const QStringList &args);
	virtual ~Part();

	static KAboutData* createAboutData();
	QWidget *widget();
	QString tikzCode() const;
	Url url() const;

protected:
	virtual bool openFile();

private slots:
	void saveAs();
//	void showJobError(KJob *job);
	void applySettings();
	void configure();

private:
	void createActions();

	TikzPreviewController *m_tikzPreviewController;
	PartConfigDialog *m_configDialog;

	KAction *m_saveAsAction;

	QString m_tikzCode;
};

#endif
