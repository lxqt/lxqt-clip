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

#include <QtCore/QSettings>
#include <QDataStream>
#include "item.h"

namespace LXQt {

class Preferences : public QSettings
{
public:
    enum PSESynchronization
    {
        PSE_NO_SYNC = 0
            , PSE_SYNC_ON_SELECTION = 1
            , PSE_SYNC_INSTANTLY = 2
    };

public:
    static const QString DEFAULT_ICON_PATH;

    static Preferences *Instance();
    ~Preferences();

    QList<Item> getStickyItems();
    void saveStickyItems(QList<Item> list);
    QList<Item> getDynamicItems();
    void saveDynamicItems(QList<Item> list);

    QString getPathToIcon() const;
    void savePathToIcon(const QString &path);

    bool trim();
    int displaySize() const;
    QString shortcut() const;
    int historyCount() const;
    bool platformExtensions() const;
    PSESynchronization synchronizePSE() const;
    bool clearItemsOnExit() const;
    bool synchronizeHistory() const;
    bool confirmOnClear() const;

    bool shouldSynchronizeClipboards() const;
    bool shouldSynchronizeClipboardsInstantly() const;

private:
    Preferences();

    static Preferences* m_instance;
};

} // namespace
