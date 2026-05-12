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

#include "preferences.h"

namespace LXQt {

const QString Preferences::DEFAULT_ICON_PATH = QStringLiteral(":/icons/lxqt-clip.png");


// allow to store ClipboardContent in the QSettings variant
QDataStream &operator<<(QDataStream &out, const ClipboardContent &obj)
{
    int size = obj.size();
    out << size;
    ClipboardContentIterator it(obj);
    while (it.hasNext())
    {
        it.next();
        out << it.key() << it.value();
    }

    return out;
}
// allow to read ClipboardContent from QSettings
QDataStream &operator>>(QDataStream &in, ClipboardContent &obj)
{
    int size;
    QString key;
    QByteArray value;
    in >> size;
    for (int i = 0; i < size; ++i)
    {
        in >> key >> value;
        obj[key] = value;
    }

    return in;
}


Preferences* Preferences::m_instance = nullptr;


Preferences::Preferences()
    : QSettings()
{
    qRegisterMetaType<ClipboardContent>("ClipboardContent");
}

Preferences::~Preferences()
{
    sync();
}

Preferences *Preferences::Instance()
{
    if (!m_instance)
        m_instance = new Preferences();
    return m_instance;
}

QList<Item> Preferences::getStickyItems()
{
    QList<Item> l;
    //
    // keys:
    //  - value, QString
    //

    beginGroup("sticky");
    int count = beginReadArray("items");
    for (int i = 0; i < count; ++i)
    {
        setArrayIndex(i);
        Item item(value("text").toString());
        if (item.isValid())
            l.append(item);
    }
    endArray();
    endGroup();

    return l;
}

void Preferences::saveStickyItems(QList<Item> list)
{
    beginGroup("sticky");
    int i = 0;
    remove("items");
    beginWriteArray("items");
    foreach (Item item, list)
    {
        setArrayIndex(i);
        i++;
        setValue("text", item.display());
    }
    endArray();
    endGroup();
}

QList<Item> Preferences::getDynamicItems()
{
    QList<Item> l;
    //
    // keys:
    //  - mode, QClipboard::Mode
    //  - contentType, Item::ContentType
    //  - content, QVariant
    //

    beginGroup("dynamic");
    int count = beginReadArray("items");
    for (int i = 0; i < count; ++i)
    {
        setArrayIndex(i);
//        qDebug() << "R" << qVariantValue<ClipboardContent>(value("content"));
        Item item(static_cast<QClipboard::Mode>(value("mode").toUInt()),//value("mode").value<QClipboard::Mode>(),
                         static_cast<Item::ContentType>(value("contentType").toUInt()), // value("contentType").value<Item::ContentType>(),
                         value("content").value<ClipboardContent>()
                        );
        if (item.isValid())
            l.append(item);
    }
    endArray();
    endGroup();

    return l;
}

void Preferences::saveDynamicItems(QList<Item> list)
{
    bool clearOnExit = clearItemsOnExit();

    beginGroup("dynamic");
    int i = 0;
    remove("items");
    if (!clearOnExit)
    {
        beginWriteArray("items");
        foreach (Item item, list)
        {
            setArrayIndex(i);
            i++;
            setValue("mode", item.clipBoardMode());
            setValue("contentType", item.contentType());
            setValue("content", QVariant::fromValue(item.content()));
        }
        endArray();
    }
    endGroup();
    sync();
}

QString Preferences::getPathToIcon() const
{
    return value(QLatin1String("tray_icon_file"), DEFAULT_ICON_PATH).toString();
}

void Preferences::savePathToIcon(const QString &path)
{
    setValue(QLatin1String("tray_icon_file"), path);
    sync();
}

bool Preferences::trim()
{
    return value("trim", true).toBool();
}

int Preferences::displaySize() const
{
    return value("displaySize", 30).toInt();
}

QString Preferences::shortcut() const
{
    return value("shortcut", "CTRL+ALT+V").toString();
}

int Preferences::historyCount() const
{
    return value("historyCount", 10).toInt();
}

bool Preferences::platformExtensions() const
{
    return value("platformExtensions", false).toBool();
}

Preferences::PSESynchronization Preferences::synchronizePSE() const
{
    return static_cast<PSESynchronization>(value("synchronizePSE", PSE_NO_SYNC).toInt());
}

bool Preferences::clearItemsOnExit() const
{
    return value("clearItemsOnExit", false).toBool();
}

bool Preferences::synchronizeHistory() const
{
    return value("synchronizeHistory", true).toBool();
}

bool Preferences::confirmOnClear() const
{
    return value("confirmClear", true).toBool();
}

bool Preferences::shouldSynchronizeClipboards() const
{
    return platformExtensions() && PSE_NO_SYNC != synchronizePSE();
}

bool Preferences::shouldSynchronizeClipboardsInstantly() const
{
    return platformExtensions() && PSE_SYNC_INSTANTLY == synchronizePSE();
}

} // namespace
