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

#include <QtGui/QClipboard>
#include <QtCore/QVariant>
#include <QtDebug>
#include "types.h"

namespace LXQt {

class Item
{
public:
    enum ContentType {
        PlainText,
        RichText,
        Image,
        Binary,
        Url,
        Sticky
    };

    enum Action
    {
        NoAction = 0
            , ToCurrent = 1
            , ToOther = 1 << 1
    };
    Q_DECLARE_FLAGS(Actions, Action)


    Item();
    Item(QClipboard::Mode mode);
    Item(QClipboard::Mode mode, ContentType contentType, const ClipboardContent &content);
    Item(const QString & sticky);

    QClipboard::Mode clipBoardMode() const;
    ClipboardContent content() const { return m_content; }
    QString display() const { return m_display; }
    Item::ContentType contentType() const { return m_contentType; }

    bool isValid() const { return m_valid; }

    void toClipboard(const Actions & actions) const;

    QString displayRole() const;
    QIcon decorationRole() const;
    QString tooltipRole() const;

    bool operator==(const Item &other) const;

private:
    QClipboard::Mode m_mode;
    ContentType m_contentType;
    bool m_valid;

    ClipboardContent m_content;
    QString m_display;

    QIcon iconForContentType() const;
};

QDebug operator<<(QDebug dbg, const Item &c);

} // namespace

Q_DECLARE_METATYPE(LXQt::Item::ContentType)
