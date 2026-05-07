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

#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QTimer>

#include "model.h"
#include "preferences.h"
#include "clipboardwrap.h"

namespace LXQt {

Model::Model(QObject *parent) :
    QAbstractListModel(parent)
{
    m_boldFont.setBold(true);

    m_sticky = Preferences::Instance()->getStickyItems();
    m_dynamic = Preferences::Instance()->getDynamicItems();
    // a little hack-a-magic to have almost
    if (m_sticky.count() + m_dynamic.count() == 0)
    {
        clipboard_changed(QClipboard::Clipboard);
        if (m_dynamic.count() == 0)
        {
            clearHistory();
        }
    }

    connect(ClipboardWrap::Instance(), &ClipboardWrap::changed, this, &Model::clipboard_changed);
}

Model::~Model()
{
    Preferences::Instance()->saveDynamicItems(m_dynamic);
    Preferences::Instance()->saveStickyItems(m_sticky);
    m_dynamic.clear();
    m_sticky.clear();
}

void Model::resetPreferences()
{
    beginRemoveRows(QModelIndex(), 0, m_sticky.count() - 1);
    m_sticky.clear();
    endRemoveRows();
    QList<Item> sticky = Preferences::Instance()->getStickyItems();
    beginInsertRows(QModelIndex(), 0, sticky.count() - 1);
    m_sticky = sticky;
    endInsertRows();
}

int Model::rowCount(const QModelIndex&) const
{
    return m_sticky.count() + m_dynamic.count();
}

// TODO/FIXME: BETTER API! This is very confusing and potentially dangerous...
QList<Item> Model::getList(int & row) const
{
    if (m_sticky.count() > row)
    {
        return m_sticky;
    }
    else
    {
        row = row - m_sticky.count();
        return m_dynamic;
    }
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return "";

    int row = index.row();

    QList<Item> list = getList(row);

    switch (role)
    {
    case Qt::DisplayRole:
        return list.at(row).displayRole();
    case Qt::DecorationRole:
        return list.at(row).decorationRole();
    case Qt::ToolTipRole:
        return list.at(row).tooltipRole();
    case Qt::FontRole:
        return m_currentIndex == index ? m_boldFont : m_normalFont;
    }

    return "";
}

Qt::ItemFlags Model::flags(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void Model::clipboard_changed(QClipboard::Mode mode)
{
    if ((mode == QClipboard::Selection || mode == QClipboard::FindBuffer)
            && !Preferences::Instance()->platformExtensions())
    {
        return;
    }

    Item item(mode);
    if (!item.isValid())
    {
        // See Item constructor: On X11 clipboard content is owned by the
        //    application, so naturally closing the application drops
        //    clipboard content. In this case the latest item should be set again.
        for (QList<Item>::const_iterator i = m_dynamic.begin(), i_e = m_dynamic.end(); i_e != i; ++i)
        {
            if (i->clipBoardMode() == item.clipBoardMode())
            {
                i->toClipboard(Item::ToCurrent);
                m_currentIndex = index(m_sticky.count() + (i - m_dynamic.begin()));
                break;
            }
        }
        return;
    }

    if (Preferences::Instance()->shouldSynchronizeClipboardsInstantly())
    {
        item.toClipboard(Item::ToOther);
    }

    // evaluate sticky items...
    int i = m_sticky.indexOf(item);
    if (i != -1)
    {
        m_currentIndex = index(i);
        return;
    }

    int ix = m_dynamic.indexOf(item);
    if (ix == -1)
    {
        const int sticky_count = m_sticky.count();
        beginInsertRows(QModelIndex(), sticky_count, sticky_count);
        m_dynamic.prepend(item);
        endInsertRows();
        const int max_history = Preferences::Instance()->historyCount();
        if (m_dynamic.count() > max_history)
        {
            beginRemoveRows(QModelIndex(), sticky_count + max_history - 1, sticky_count + m_dynamic.count() - 1);
            m_dynamic.erase(m_dynamic.begin() + (max_history - 1), m_dynamic.end());
            endRemoveRows();
        }
        ix = 0;
    }
    setCurrentDynamic(ix);
}

void Model::setCurrentDynamic(int ix)
{
    // move if not already on top
    if (ix != 0)
    {
        const int sticky_count = m_sticky.count();
        beginMoveRows(QModelIndex(), sticky_count + ix, sticky_count + ix, QModelIndex(), sticky_count);
        m_dynamic.move(ix, 0);
        endMoveRows();
    }

    m_currentIndex = index(m_sticky.count());

    if (Preferences::Instance()->synchronizeHistory())
    {
        Preferences::Instance()->saveDynamicItems(m_dynamic);
    }
}


void Model::clearHistory()
{
    const int sticky_count = m_sticky.count();
    beginRemoveRows(QModelIndex(), sticky_count, sticky_count + m_dynamic.count() - 1);
    m_dynamic.clear();
    endRemoveRows();
    ClipboardContent tmp;
    tmp["text/plain"] = tr("Welcome to the lxqt-clip clipboard history applet").toUtf8();
    Item item(QClipboard::Clipboard, Item::PlainText, tmp);
    beginInsertRows(QModelIndex(), sticky_count, sticky_count);
    m_dynamic.append(item);
    endInsertRows();
    m_currentIndex = index(sticky_count);
}

void Model::indexTriggered(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    QList<Item> list = getList(row);
    Item::Actions actions(Item::ToCurrent);
    actions |= Preferences::Instance()->shouldSynchronizeClipboards() ? Item::ToOther : Item::NoAction;
    list.at(row).toClipboard(actions);
    m_currentIndex = index;
    if (m_sticky.size() <= index.row())
    {
        setCurrentDynamic(row);
    }
}

} // namespace
