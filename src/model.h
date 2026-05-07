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

#include <QAbstractListModel>
#include <QtGui/QClipboard>
#include <QtGui/QFont>

#include "item.h"

class QTimer;

namespace LXQt {

class Model : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);
    ~Model();

    void resetPreferences();

public slots:
    void clearHistory();
    void indexTriggered(const QModelIndex &);

protected:
    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

private:
    QList<Item> m_sticky;
    QList<Item> m_dynamic;
    QPersistentModelIndex m_currentIndex;

    QFont m_normalFont;
    QFont m_boldFont;

    QList<Item> getList(int &row) const;
    void setCurrentDynamic(int ix);

private slots:
    void clipboard_changed(QClipboard::Mode);
};

} // namespace
