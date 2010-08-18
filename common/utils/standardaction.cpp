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

#include "standardaction.h"

#ifdef KTIKZ_USE_KDE
#include <KActionCollection>
#include <KRecentFilesAction>
#include <KStandardAction>
#else
#include <QCoreApplication>
#endif
#include "icon.h"

namespace StandardAction
{
#ifdef KTIKZ_USE_KDE
// XXX the following is an ugly hack, but I don't know how to promote a KAction to an Action
Action *copyAction(KAction *action, const QObject *recvr, const char *slot)
{
	Action *newAction = new Action(action->icon(), action->text(), action->parent());
	newAction->setShortcut(action->shortcut());
	newAction->setData(action->data());
	newAction->setObjectName(action->objectName());
	QObject::connect(newAction, SIGNAL(triggered()), recvr, slot);
	Action::actionCollection()->addAction(newAction->objectName(), newAction);
	return newAction;
}

Action *openNew(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::openNew(recvr, slot, parent), recvr, slot);
}
/*
KAction *openNew(const QObject *recvr, const char *slot, QObject *parent)
{
	KAction *action = KStandardAction::openNew(recvr, slot, parent);
	Action::actionCollection()->addAction(action->objectName(), action);
	return action;
}
*/
Action *open(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::open(recvr, slot, parent), recvr, slot);
}
RecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent)
{
	KRecentFilesAction *action = KStandardAction::openRecent(recvr, slot, parent);
	RecentFilesAction *newAction = new RecentFilesAction(action->icon(), action->text(), action->parent());
	newAction->setShortcut(action->shortcut());
	newAction->setData(action->data());
	newAction->setObjectName(action->objectName());
	newAction->setToolBarMode(KRecentFilesAction::MenuMode);
	newAction->setToolButtonPopupMode(QToolButton::MenuButtonPopup);
	QObject::connect(newAction, SIGNAL(urlSelected(Url)), recvr, slot);
	Action::actionCollection()->addAction(newAction->objectName(), newAction);
	return newAction;
}
/*
KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent)
{
	KRecentFilesAction *action = KStandardAction::openRecent(recvr, slot, parent);
	action->setToolBarMode(KRecentFilesAction::MenuMode);
	action->setToolButtonPopupMode(QToolButton::MenuButtonPopup);
	Action::actionCollection()->addAction(action->objectName(), action);
	return action;
}
*/
Action *save(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::save(recvr, slot, parent), recvr, slot);
}
Action *saveAs(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::saveAs(recvr, slot, parent), recvr, slot);
}
Action *close(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::close(recvr, slot, parent), recvr, slot);
}
Action *quit(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::quit(recvr, slot, parent), recvr, slot);
}
Action *undo(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::undo(recvr, slot, parent), recvr, slot);
}
Action *redo(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::redo(recvr, slot, parent), recvr, slot);
}
Action *cut(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::cut(recvr, slot, parent), recvr, slot);
}
Action *copy(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::copy(recvr, slot, parent), recvr, slot);
}
Action *paste(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::paste(recvr, slot, parent), recvr, slot);
}
Action *selectAll(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::selectAll(recvr, slot, parent), recvr, slot);
}
Action *find(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::find(recvr, slot, parent), recvr, slot);
}
Action *findNext(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::findNext(recvr, slot, parent), recvr, slot);
}
Action *findPrev(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::findPrev(recvr, slot, parent), recvr, slot);
}
Action *replace(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::replace(recvr, slot, parent), recvr, slot);
}
Action *gotoLine(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::gotoLine(recvr, slot, parent), recvr, slot);
}
Action *zoomIn(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::zoomIn(recvr, slot, parent), recvr, slot);
}
Action *zoomOut(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::zoomOut(recvr, slot, parent), recvr, slot);
}
Action *preferences(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::preferences(recvr, slot, parent), recvr, slot);
}
#else
Action *createAction(int which, const QObject *recvr, const char *slot, QObject *parent)
{
	QString iconName;
	QString text;
	QKeySequence::StandardKey key = QKeySequence::UnknownKey;

	switch (which)
	{
		case 0: iconName = "document-new"; text = QObject::tr("&New"); key = QKeySequence::New; break;
		case 1: iconName = "document-open"; text = QObject::tr("&Open..."); key = QKeySequence::Open; break;
		case 2: iconName = "document-save"; text = QObject::tr("&Save"); key = QKeySequence::Save; break;
		case 3: iconName = "document-save-as"; text = QObject::tr("Save &As..."); break;
		case 4: iconName = "window-close"; text = QObject::tr("&Close"); key = QKeySequence::Close; break;
		case 5: iconName = "application-exit"; text = QObject::tr("&Quit"); break;
		case 6: iconName = "edit-undo"; text = QObject::tr("&Undo"); key = QKeySequence::Undo; break;
		case 7: iconName = "edit-redo"; text = QObject::tr("Re&do"); key = QKeySequence::Redo; break;
		case 8: iconName = "edit-cut"; text = QObject::tr("Cu&t"); key = QKeySequence::Cut; break;
		case 9: iconName = "edit-copy"; text = QObject::tr("&Copy"); key = QKeySequence::Copy; break;
		case 10: iconName = "edit-paste"; text = QObject::tr("&Paste"); key = QKeySequence::Paste; break;
		case 11: text = QObject::tr("Select &All"); key = QKeySequence::SelectAll; break;
		case 12: iconName = "edit-find"; text = QObject::tr("&Find..."); key = QKeySequence::Find; break;
		case 13: iconName = "go-down"; text = QObject::tr("Find &Next"); key = QKeySequence::FindNext; break;
		case 14: iconName = "go-up"; text = QObject::tr("Find Pre&vious"); key = QKeySequence::FindPrevious; break;
		case 15: text = QObject::tr("&Replace..."); key = QKeySequence::Replace; break;
		case 16: iconName = "go-jump"; text = QObject::tr("&Go to Line..."); break;
		case 17: iconName = "zoom-in"; text = QObject::tr("Zoom &In"); key = QKeySequence::ZoomIn; break;
		case 18: iconName = "zoom-out"; text = QObject::tr("Zoom &Out"); key = QKeySequence::ZoomOut; break;
		case 19: iconName = "configure"; text = QObject::tr("&Configure %1...").arg(QCoreApplication::applicationName()); break;
	}

	Action *action;
	if (!iconName.isEmpty())
		action = new Action(Icon(iconName), text, parent);
	else
		action = new Action(text, parent);

	if (which == 5)
		action->setShortcut(QObject::tr("Ctrl+Q", "File|Quit"));
	else if (which == 16)
		action->setShortcut(QObject::tr("Ctrl+G", "Edit|Go to Line"));
	else if (which != 3 && which != 19)
		action->setShortcut(key);
	QObject::connect(action, SIGNAL(triggered()), recvr, slot);
	return action;
}
Action *openNew(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(0, recvr, slot, parent);
}
Action *open(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(1, recvr, slot, parent);
}
RecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent)
{
	RecentFilesAction *action = new RecentFilesAction(Icon("document-open-recent"), QObject::tr("&Open Recent"), parent);
	QObject::connect(action, SIGNAL(urlSelected(Url)), recvr, slot);
	return action;
}
Action *save(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(2, recvr, slot, parent);
}
Action *saveAs(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(3, recvr, slot, parent);
}
Action *close(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(4, recvr, slot, parent);
}
Action *quit(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(5, recvr, slot, parent);
}
Action *undo(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(6, recvr, slot, parent);
}
Action *redo(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(7, recvr, slot, parent);
}
Action *cut(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(8, recvr, slot, parent);
}
Action *copy(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(9, recvr, slot, parent);
}
Action *paste(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(10, recvr, slot, parent);
}
Action *selectAll(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(11, recvr, slot, parent);
}
Action *find(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(12, recvr, slot, parent);
}
Action *findNext(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(13, recvr, slot, parent);
}
Action *findPrev(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(14, recvr, slot, parent);
}
Action *replace(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(15, recvr, slot, parent);
}
Action *gotoLine(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(16, recvr, slot, parent);
}
Action *zoomIn(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(17, recvr, slot, parent);
}
Action *zoomOut(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(18, recvr, slot, parent);
}
Action *preferences(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(19, recvr, slot, parent);
}
#endif
}
