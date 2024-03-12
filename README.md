[![Build Status](https://github.com/fhackenberger/ktikz/workflows/CI/badge.svg)](https://github.com/fhackenberger/ktikz/actions?query=branch%3Amaster+workflow%3ACI)

# KtikZ

KtikZ provides a nice user interface for making pictures using TikZ

# Usage

You write the TikZ code in the editor and KtikZ automatically generates the
image in a preview panel.

The complete description of the usage is available in the application itself,
under Help -> KtikZ Manual.

# Installation

See the INSTALL file.

# Screenshots

The main window showing the application in action:

![The main window showing the application in action](screenshots/screenshot_v0.13.png)

# License

```
Copyright (C) 2007-2011 Florian Hackenberger <florian@hackenberger.at>
Copyright (C) 2007-2013 Glad Deschrijver <glad.deschrijver@gmail.com>
Copyright (C) 2013 by João Carreira <jfmcarreira@gmail.com>

License: GPL version 2 or later

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your option) any later
  version.

  This program links to poppler which is based on xpdf code, which currently is
  released under the GPL version 2 only.  As long as xpdf is not released under
  the GPL version 2 or later, this program cannot be released under the GPL
  version 3.  Currently there are no other restrictions for this move.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

Additional copyright holders and licenses:
  app/tikzeditor.cpp:
    GPL v2 or later contributions:
      Copyright (C) 2006 Jean-Luc Biord <jlbiord@gmail.com> (http://qdevelop.org)
      Copyright (C) 2003-2008 Pascal Brachet <pbrachet@xm1math.net> (http://www.xm1math.net/texmaker)
    LGPL v2.1 contributions:
      Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies) <qt-info@nokia.com>
    LGPL v2 contributions:
      Copyright (C) 2007 Mirko Stocker <me@misto.ch>
      Copyright (C) 2003-2005 Hamish Rodda <rodda@kde.org>
      Copyright (C) 2001 Christoph Cullmann <cullmann@kde.org>
      Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>
      Copyright (C) 1999 Jochen Wilhelmy <digisnap@cs.tu-berlin.de>

  part/part.cpp:
    GPL v2 or later contributions:
      Copyright (C) 2002 Wilco Greven <greven@kde.org>
      Copyright (C) 2002 Chris Cheney <ccheney@cheney.cx>
      Copyright (C) 2002 Malcolm Hunter <malcolm.hunter@gmx.co.uk>
      Copyright (C) 2003-2004 Christophe Devriese <Christophe.Devriese@student.kuleuven.ac.be>
      Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
      Copyright (C) 2003 Andy Goossens <andygoossens@telenet.be>
      Copyright (C) 2003 Dirk Mueller <mueller@kde.org>
      Copyright (C) 2003 Laurent Montel <montel@kde.org>
      Copyright (C) 2004 Dominique Devriese <devriese@kde.org>
      Copyright (C) 2004 Christoph Cullmann <crossfire@babylon2k.de>
      Copyright (C) 2004 Henrique Pinto <stampede@coltec.ufmg.br>
      Copyright (C) 2004 Waldo Bastian <bastian@kde.org>
      Copyright (C) 2004-2008 Albert Astals Cid <aacid@kde.org>
      Copyright (C) 2004 Antti Markus <antti.markus@starman.ee>

  common/utils/colorbutton.cpp:
    GPL v2 or later contributions:
      Copyright (C) 2004 Ariya Hidayat <ariya@kde.org> (http://code.google.com/p/speedcrunch/)

  common/utils/lineedit.{h,cpp}:
    Use, modification and distribution is allowed without limitation,
    warranty, liability or support of any kind.
      Copyright (C) 2007 Trolltech ASA <info@trolltech.com>

  doc/*:
    GFDL v1.2 or later contributions:
      Permission is granted to copy, distribute and/or modify this document
      under the terms of the GNU Free Documentation License, Version 1.2 or
      any later version published by the Free Software Foundation; with no
      Invariant Sections, with no Front-Cover Texts, and with no Back-Cover
      Texts.
      Copyright (C) 2010-2011 Glad Deschrijver <glad.deschrijver@gmail.com>

  app/icons/Prociono.otf
    This font is used in the QtikZ and KtikZ icons.

    This font has been released into the public domain by its author,
    Barry Schwartz. This applies worldwide.

    In some countries this may not be legally possible; if so:

    Barry Schwartz grants anyone the right to use this work for any
    purpose, without any conditions, unless such conditions are required
    by law.

  app/icons/qt-logo-22.png
    LGPL v2.1 contributions:
      Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies) <qt-info@nokia.com>

  app/icons/*
    The icons are copied or derived from KDE's Oxygen icon set which is released
    under the LGPL (see http://www.oxygen-icons.org/?page_id=4).
    Upstream authors and copyright holders:
        Copyright (C) 2007-2009 David Vignoni <david@icon-king.com>
        Copyright (C) 2007-2009 Johann Ollivier Lapeyre <johann@oxygen-icons.org>
        Copyright (C) 2007-2009 Kenneth Wimer <kwwii@bootsplash.org>
        Copyright (C) 2007-2009 Nuno Fernades Pinheiro <nf.pinheiro@gmail.com>
        Copyright (C) 2007-2009 Riccardo Iaconelli <riccardo@oxygen-icons.org>
        Copyright (C) 2007-2009 David Miller <miller@oxygen-icons.org>
    and others.

    License:
        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Library General Public
        License as published by the Free Software Foundation; either
        version 3 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Library General Public License for more details.

  translations/cs/ktikz.po, translations/cs/qtikz_cs.ts
    The translations are licenced under the same terms as ktikz itself.
      Copyright (C) 2010 Pavel Fric <pavelfric@seznam.cz>

  translations/*/ktikz.po, translations/*/qtikz_*.ts
    The translations are licenced under the same terms as ktikz itself.
      Copyright (C) 2007-2011 Florian Hackenberger <florian@hackenberger.at>
      Copyright (C) 2007-2013 Glad Deschrijver <glad.deschrijver@gmail.com>
```
