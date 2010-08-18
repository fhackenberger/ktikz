/***************************************************************************
 *   Copyright (C) 2009, 2010 by Glad Deschrijver                          *
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

#ifndef KTIKZAPPLICATION_H
#define KTIKZAPPLICATION_H

#ifdef KTIKZ_USE_KDE
#include <KApplication>
#else
#include <QApplication>
#include <QStringList>
#endif

class MainWindow;

#ifdef KTIKZ_USE_KDE
class KtikzApplication : public KApplication
#else
class KtikzApplication : public QApplication
#endif
{
	Q_OBJECT

public:
	KtikzApplication(int &argc, char **argv);
	virtual ~KtikzApplication();

	void init();
	static QString applicationName();

#ifndef KTIKZ_USE_KDE
protected:
	void commitData(QSessionManager &manager);
	void saveState(QSessionManager &manager);
#endif

private:
#ifndef KTIKZ_USE_KDE
	QStringList m_args;
#endif
	bool m_firstTime;
};

#endif
