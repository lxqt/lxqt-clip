/*
lxqt-clip - clipboard history manager
Copyright (C) 2026 Palo Kisa <palo.kisa@gmail.com>
              2026~ LXQt team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once

#include <QObject>

namespace LXQt {

class Systray;

class DbusInterface : public QObject
{
    Q_OBJECT

    // Note: avoiding lxqt-clip as dash(-) in inteface name is causing problems in DBus ecosystem
    Q_CLASSINFO("D-Bus Interface", "org.lxqt.clip")

public:
    explicit DbusInterface(Systray & tray, QObject * parent = nullptr);
public slots:
    void show();
private:
    Systray & m_tray;
};

} // namespace
