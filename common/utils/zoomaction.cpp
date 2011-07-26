/***************************************************************************
 *   Copyright (C) 2009, 2011 by Glad Deschrijver                          *
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

#include "zoomaction.h"

#include "globallocale.h"
#include "icon.h"

static const qreal s_minZoomFactor = 0.1;
static const qreal s_maxZoomFactor = 6;

ZoomAction::ZoomAction(QObject *parent, const QString &name)
	: SelectAction(parent, name)
{
	init();
}

ZoomAction::ZoomAction(const QString &text, QObject *parent, const QString &name)
	: SelectAction(text, parent, name)
{
	init();
}

ZoomAction::ZoomAction(const Icon &icon, const QString &text, QObject *parent, const QString &name)
	: SelectAction(icon, text, parent, name)
{
	init();
}

void ZoomAction::init()
{
	setEditable(true);
	setToolTip(tr("Select or insert zoom factor here"));
	setWhatsThis(tr("<p>Select the zoom factor here.  "
	                "Alternatively, you can also introduce a zoom factor and "
	                "press Enter.</p>"));
	setCurrentZoomFactor();
	connect(this, SIGNAL(triggered(QString)), this, SLOT(setZoomFactor(QString)));
}

ZoomAction::~ZoomAction()
{
}

qreal ZoomAction::minZoomFactor() const
{
	return s_minZoomFactor;
}

qreal ZoomAction::maxZoomFactor() const
{
	return s_maxZoomFactor;
}

QString ZoomAction::formatZoomFactor(qreal zoomFactor) const
{
	QString zoomFactorText = GlobalLocale::formatNumber(zoomFactor, 2);
	const QString decimalSymbol = GlobalLocale::decimalSymbol();

	zoomFactorText.remove(decimalSymbol + "00");
	// remove trailing zero in numbers like 12.30
	if (zoomFactorText.endsWith('0')
	        && zoomFactorText.indexOf(decimalSymbol) >= 0)
		zoomFactorText.chop(1);

	zoomFactorText += '%';
	return zoomFactorText;
}

void ZoomAction::setCurrentZoomFactor(qreal newZoomFactor)
{
	const qreal zoomFactorArray[] = {12.50, 25, 50, 75, 100, 125, 150, 200, 250, 300};
	const int zoomFactorNumber = 10;
	QStringList zoomFactorList;
	int newZoomFactorPosition = -1;
	bool addNewZoomFactor = true;

	if (newZoomFactor < s_minZoomFactor || newZoomFactor > s_maxZoomFactor)
		addNewZoomFactor = false;

	newZoomFactor *= 100;
	for (int i = 0; i < zoomFactorNumber; ++i)
	{
		if (addNewZoomFactor && newZoomFactor < zoomFactorArray[i])
		{
			zoomFactorList << formatZoomFactor(newZoomFactor);
			newZoomFactorPosition = i;
			addNewZoomFactor = false;
		}
		else if (newZoomFactor == zoomFactorArray[i])
		{
			newZoomFactorPosition = i;
			addNewZoomFactor = false;
		}
		zoomFactorList << formatZoomFactor(zoomFactorArray[i]);
	}
	if (addNewZoomFactor)
	{
		zoomFactorList << formatZoomFactor(newZoomFactor);
		newZoomFactorPosition = zoomFactorNumber;
	}

	disconnect(this, SIGNAL(triggered(QString)), this, SLOT(setZoomFactor(QString)));
	removeAllActions();
	setItems(zoomFactorList);
	if (newZoomFactorPosition >= 0)
		setCurrentItem(newZoomFactorPosition);
	connect(this, SIGNAL(triggered(QString)), this, SLOT(setZoomFactor(QString)));
}

void ZoomAction::setZoomFactor(qreal zoomFactor)
{
	// adjust zoom factor
	zoomFactor = qBound(s_minZoomFactor, zoomFactor, s_maxZoomFactor);

	// add current zoom factor to the list of zoom factors
	const QString zoomFactorString = formatZoomFactor(zoomFactor * 100);
	const int zoomFactorIndex = items().indexOf(zoomFactorString);
	if (zoomFactorIndex >= 0)
		setCurrentItem(zoomFactorIndex);
	else
		setCurrentZoomFactor(zoomFactor);

	emit zoomFactorAdded(zoomFactor);
}

void ZoomAction::setZoomFactor(const QString &zoomFactorText)
{
	setZoomFactor(GlobalLocale::readNumber(QString(zoomFactorText).remove(QRegExp(QString("[^\\d\\%1]*").arg(GlobalLocale::decimalSymbol())))) / 100.0);
}
