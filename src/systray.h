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

#pragma once

#include <QSystemTrayIcon>

class QMenuView;
#ifndef NO_QXT
class QxtGlobalShortcut;
#endif

namespace LXQt {
class Model;

class Systray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit Systray(QObject *parent = 0);
    ~Systray();

private:
    Model *m_model;
    QMenu *m_contextMenu;
#ifndef NO_QXT
    QMenuView *m_shortcutMenu;
    QxtGlobalShortcut *m_shortcut;
#endif

public slots:
    void shortcut_activated();
private slots:
    void editPreferences();
    void showAbout();
    void systray_activated(QSystemTrayIcon::ActivationReason reason);
    void clear_history();
};

} // namespace
