/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014                      *
 *     by Glad Deschrijver <glad.deschrijver@gmail.com>                    *
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

#ifndef TIKZEDITORVIEWABSTRACT_H
#define TIKZEDITORVIEWABSTRACT_H

#include <QWidget>

class TikzEditorViewAbstract: public QWidget
{
public:
  explicit TikzEditorViewAbstract(QWidget *parent = 0)
        : QWidget(parent)
    {}
  virtual ~TikzEditorViewAbstract(){ };

  virtual QString text() = 0;
  virtual void updateCompleter(bool useCompletion, const QStringList &words) = 0;
  virtual void clear() = 0;
  virtual bool isEmpty() = 0;
  virtual bool isModified() = 0;
  virtual void setModified( bool value ) = 0;

};

#endif
