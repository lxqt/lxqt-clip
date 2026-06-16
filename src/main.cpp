/*
lxqt-clip - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@yarpen.cz>
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

#include <LXQt/SingleApplication>

#include <QSettings>
#include <QTranslator>
#include <QtDebug>
#include <QSharedMemory>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusError>

#include "systray.h"
#include "preferences.h"
#include "dbusinterface.h"

int main(int argc, char **argv)
{
    LXQt::SingleApplication a(argc, argv);

    a.setApplicationName(QLatin1String{"lxqt-clip"});
    a.setDesktopFileName(QLatin1String{"lxqt-clip"});
    a.setApplicationVersion(QLatin1String{LXQTCLIP_VERSION});
    a.setOrganizationDomain(QLatin1String{"lxqt.org"});
    a.setOrganizationName(QLatin1String{"lxqt"});

    QSettings::setDefaultFormat(QSettings::IniFormat);

    a.setQuitOnLastWindowClosed(false);
    a.setWindowIcon(QIcon{LXQt::Preferences::Instance()->getPathToIcon()});

    LXQt::Systray s;

    // Note: Allow only one instance of lxqt-clip.
    LXQt::DbusInterface dbus_i{s};
    auto conn = QDBusConnection::sessionBus();
    if (!conn.registerService(QLatin1String{"org.lxqt.lxqt-clip"}))
    {
        qWarning() << "Can't register D-Bus service";
        qWarning("An instance of lxqt-clip is already running!");
        return 1;
    }
    // Note: avoiding lxqt-clip as dash(-) in object path is causing problems in DBus ecosystem
    if (!conn.registerObject(QLatin1String{"/org/lxqt/clip"}, &dbus_i, QDBusConnection::ExportAllSlots))
        qWarning() << "Can't register object:" << conn.lastError().message();

    s.show();
    return a.exec();
}
