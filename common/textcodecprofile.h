/***************************************************************************
 *   Copyright (C) 2016 by G. Prudhomme                                    *
 *     <gprud@users.noreply.github.com>                                    *
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

#ifndef TEXTCODECPROFILE_H
#define TEXTCODECPROFILE_H

class QTextStream;

class TextCodecProfile
{
public:
    /// Configure a QTextStream to encode a TeX file.
    /// @arg textStream A non-null instance of QTextStream.
    void configureStreamEncoding(QTextStream &textStream) const { Q_UNUSED(textStream); }
    /// Configure a QTextStream to decode a TeX file.
    /// @arg textStream A non-null instance of QTextStream.
    void configureStreamDecoding(QTextStream &textStream) const { Q_UNUSED(textStream); }
};

#endif // TEXTCODECPROFILE_H
