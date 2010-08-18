/***************************************************************************
 *   Copyright (C) 2009 by Glad Deschrijver                                *
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

#ifndef KTIKZ_STANDARDACTION_H
#define KTIKZ_STANDARDACTION_H

#include "action.h"
#include "recentfilesaction.h"

namespace StandardAction
{
#ifdef KTIKZ_USE_KDE
	Action *copyAction(KAction *action, const QObject *recvr, const char *slot);
#else
	Action *createAction(int which, const QObject *recvr, const char *slot, QObject *parent);
#endif
	Action *openNew(const QObject *recvr, const char *slot, QObject *parent);
	Action *open(const QObject *recvr, const char *slot, QObject *parent);
	RecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent);
	Action *save(const QObject *recvr, const char *slot, QObject *parent);
	Action *saveAs(const QObject *recvr, const char *slot, QObject *parent);
	Action *close(const QObject *recvr, const char *slot, QObject *parent);
	Action *quit(const QObject *recvr, const char *slot, QObject *parent);
	Action *undo(const QObject *recvr, const char *slot, QObject *parent);
	Action *redo(const QObject *recvr, const char *slot, QObject *parent);
	Action *cut(const QObject *recvr, const char *slot, QObject *parent);
	Action *copy(const QObject *recvr, const char *slot, QObject *parent);
	Action *paste(const QObject *recvr, const char *slot, QObject *parent);
	Action *selectAll(const QObject *recvr, const char *slot, QObject *parent);
	Action *find(const QObject *recvr, const char *slot, QObject *parent);
	Action *findNext(const QObject *recvr, const char *slot, QObject *parent);
	Action *findPrev(const QObject *recvr, const char *slot, QObject *parent);
	Action *replace(const QObject *recvr, const char *slot, QObject *parent);
	Action *gotoLine(const QObject *recvr, const char *slot, QObject *parent);
	Action *zoomIn(const QObject *recvr, const char *slot, QObject *parent);
	Action *zoomOut(const QObject *recvr, const char *slot, QObject *parent);
	Action *preferences(const QObject *recvr, const char *slot, QObject *parent);
}

#endif
