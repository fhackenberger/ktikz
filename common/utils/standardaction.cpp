/***************************************************************************
 *   Copyright (C) 2009, 2012 by Glad Deschrijver                          *
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
#include <QtCore/QCoreApplication>
#endif

#include "icon.h"
#include "action.h"
#include "recentfilesaction.h"

namespace StandardAction
{
#ifdef KTIKZ_USE_KDE
// XXX the following is an ugly hack, but I don't know how to promote a QAction to an Action
static Action *copyAction(QAction *action, const QObject *recvr, const char *slot)
{
	Action *newAction = new Action(Icon(action->icon()), action->text(), action->parent());
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
QAction *openNew(const QObject *recvr, const char *slot, QObject *parent)
{
	QAction *action = KStandardAction::openNew(recvr, slot, parent);
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
	RecentFilesAction *newAction = new RecentFilesAction(Icon(action->icon()), action->text(), action->parent());
	newAction->setShortcut(action->shortcut());
	newAction->setData(action->data());
	newAction->setObjectName(action->objectName());
	newAction->setToolBarMode(KRecentFilesAction::MenuMode);
	newAction->setToolButtonPopupMode(QToolButton::MenuButtonPopup);
	QObject::connect(newAction, SIGNAL(urlSelected(QUrl)), recvr, slot);
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
Action *printPreview(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::printPreview(recvr, slot, parent), recvr, slot);
}
Action *print(const QObject *recvr, const char *slot, QObject *parent)
{
	return copyAction(KStandardAction::print(recvr, slot, parent), recvr, slot);
}
#else
static Action *createAction(int which, const QObject *recvr, const char *slot, QObject *parent)
{
	QString iconName;
	QString text;
	QKeySequence::StandardKey key = QKeySequence::UnknownKey;

	switch (which)
	{
		case 0:
			iconName = QLatin1String("document-new");
			text = QCoreApplication::translate("StandardAction", "&New");
			key = QKeySequence::New;
			break;
		case 1:
			iconName = QLatin1String("document-open");
			text = QCoreApplication::translate("StandardAction", "&Open...");
			key = QKeySequence::Open;
			break;
		case 2:
			iconName = QLatin1String("document-save");
			text = QCoreApplication::translate("StandardAction", "&Save");
			key = QKeySequence::Save;
			break;
		case 3:
			iconName = QLatin1String("document-save-as");
			text = QCoreApplication::translate("StandardAction", "Save &As...");
			break;
		case 4:
			iconName = QLatin1String("window-close");
			text = QCoreApplication::translate("StandardAction", "&Close");
			key = QKeySequence::Close;
			break;
		case 5:
			iconName = QLatin1String("application-exit");
			text = QCoreApplication::translate("StandardAction", "&Quit");
			break;
		case 6:
			iconName = QLatin1String("edit-undo");
			text = QCoreApplication::translate("StandardAction", "&Undo");
			key = QKeySequence::Undo;
			break;
		case 7:
			iconName = QLatin1String("edit-redo");
			text = QCoreApplication::translate("StandardAction", "Re&do");
			key = QKeySequence::Redo;
			break;
		case 8:
			iconName = QLatin1String("edit-cut");
			text = QCoreApplication::translate("StandardAction", "Cu&t");
			key = QKeySequence::Cut;
			break;
		case 9:
			iconName = QLatin1String("edit-copy");
			text = QCoreApplication::translate("StandardAction", "&Copy");
			key = QKeySequence::Copy;
			break;
		case 10:
			iconName = QLatin1String("edit-paste");
			text = QCoreApplication::translate("StandardAction", "&Paste");
			key = QKeySequence::Paste;
			break;
		case 11:
			text = QCoreApplication::translate("StandardAction", "Select &All");
			key = QKeySequence::SelectAll;
			break;
		case 12:
			iconName = QLatin1String("edit-find");
			text = QCoreApplication::translate("StandardAction", "&Find...");
			key = QKeySequence::Find;
			break;
		case 13:
			iconName = QLatin1String("go-down");
			text = QCoreApplication::translate("StandardAction", "Find &Next");
			key = QKeySequence::FindNext;
			break;
		case 14:
			iconName = QLatin1String("go-up");
			text = QCoreApplication::translate("StandardAction", "Find Pre&vious");
			key = QKeySequence::FindPrevious;
			break;
		case 15:
			iconName = QLatin1String("edit-find-replace");
			text = QCoreApplication::translate("StandardAction", "&Replace...");
			key = QKeySequence::Replace;
			break;
		case 16:
			iconName = QLatin1String("go-jump");
			text = QCoreApplication::translate("StandardAction", "&Go to Line...");
			break;
		case 17:
			iconName = QLatin1String("zoom-in");
			text = QCoreApplication::translate("StandardAction", "Zoom &In");
			key = QKeySequence::ZoomIn;
			break;
		case 18:
			iconName = QLatin1String("zoom-out");
			text = QCoreApplication::translate("StandardAction", "Zoom &Out");
			key = QKeySequence::ZoomOut;
			break;
		case 19:
			iconName = QLatin1String("configure");
			text = QCoreApplication::translate("StandardAction", "&Configure %1...").arg(QCoreApplication::applicationName());
			break;
		case 20:
			iconName = QLatin1String("document-print-preview");
			text = QCoreApplication::translate("StandardAction", "Print Pre&view...");
			break;
		case 21:
			iconName = QLatin1String("document-print");
			text = QCoreApplication::translate("StandardAction", "&Print...");
			key = QKeySequence::Print;
			break;
	}

	Action *action;
	if (!iconName.isEmpty())
		action = new Action(Icon(iconName), text, parent);
	else
		action = new Action(text, parent);

	if (which == 5)
		action->setShortcut(QCoreApplication::translate("StandardAction", "Ctrl+Q", "File|Quit"));
	else if (which == 16)
		action->setShortcut(QCoreApplication::translate("StandardAction", "Ctrl+G", "Edit|Go to Line"));
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
	RecentFilesAction *action = new RecentFilesAction(Icon(QLatin1String("document-open-recent")), QCoreApplication::translate("StandardAction", "&Open Recent"), parent);
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
Action *printPreview(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(20, recvr, slot, parent);
}
Action *print(const QObject *recvr, const char *slot, QObject *parent)
{
	return createAction(21, recvr, slot, parent);
}
#endif
}
