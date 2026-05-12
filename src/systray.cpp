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
#include <QMessageBox>
#include <QDebug>

#include "qmenuview.h"
#include "model.h"
#include "qxtglobalshortcut.h"
#include "preferences.h"
#include "preferencesdialog.h"
#include "systray.h"


namespace LXQt {

Systray::Systray(QObject *parent)
    : QSystemTrayIcon(parent)
#ifndef NO_QXT
      , m_shortcutMenu(0)
#endif
{
    setIcon(QIcon(Preferences::Instance()->getPathToIcon()));

    m_model = new Model(this);

    m_contextMenu = new QMenu();
    m_contextMenu->addAction(QIcon::fromTheme("edit-clear-hstory", QIcon(":/icons/edit-clear-history.png")), tr("C&lear clipboard history")
            , this, &Systray::clear_history);
    m_contextMenu->addAction(QIcon::fromTheme("configure", QIcon(":/icons/configure.png")), tr("&Configure...")
            , this, &Systray::editPreferences);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(QIcon::fromTheme("help-about", QIcon(":/icons/help-about.png")), tr("&About ...")
            , this, &Systray::showAbout);
    m_contextMenu->addAction(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.png")), tr("&Quit")
            , qApp, &QCoreApplication::quit);
    setContextMenu(m_contextMenu);

#ifndef NO_QXT
    m_shortcutMenu = new QMenuView();
    m_shortcutMenu->setModel(m_model);
    m_shortcutMenu->setWindowTitle(tr("lxqt-clip - a clipboard history applet"));

    // This flag is mandatory to get focus when user activates global_key.
    // OK, window gets a decoration but it works. Menu is displayed without the
    // the decoration if is the systray icon clicked.
    m_shortcutMenu->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);

    connect(m_shortcutMenu, SIGNAL(triggered(QModelIndex)), m_model, SLOT(indexTriggered(QModelIndex)));
    connect(m_shortcutMenu, SIGNAL(triggered(QModelIndex)), m_shortcutMenu, SLOT(close()));

    m_shortcut = new QxtGlobalShortcut(this);
    connect(m_shortcut, SIGNAL(activated()), this, SLOT(shortcut_activated()));
    m_shortcut->setShortcut(Preferences::Instance()->shortcut());
#else
    qWarning() << "Global keyboard shortcut is not compiled in.";
#endif

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(systray_activated(QSystemTrayIcon::ActivationReason)));
}

Systray::~Systray()
{
#ifndef NO_QXT
    m_contextMenu->deleteLater();
    if (m_shortcutMenu)
        m_shortcutMenu->deleteLater();
#endif
    m_model->deleteLater();
}

void Systray::shortcut_activated()
{
#ifndef NO_QXT
    if (m_shortcutMenu->isVisible())
        m_shortcutMenu->hide();
    else
    {
        m_shortcutMenu->popup(QCursor::pos());
        // activateWindow and raise are mandatory to get proper focus for keyboard after global_key.
        m_shortcutMenu->activateWindow();
        m_shortcutMenu->raise();
    }
#endif
}

void Systray::editPreferences()
{
    PreferencesDialog d;
    if (!d.exec())
        return;

#ifndef NO_QXT
    m_shortcut->setShortcut(Preferences::Instance()->shortcut());
#endif
    m_model->resetPreferences();

    // Set new icon.
    const QString icon_path = Preferences::Instance()->getPathToIcon();
    setIcon(QIcon(icon_path));
}

void Systray::showAbout()
{
#ifndef NO_QXT
    QString globalKeySupport = tr("Yes");
#else
    QString globalKeySupport = tr("No");
#endif

    QMessageBox msgBox;
    //msgBox.setMinimumWidth(600);
    msgBox.setWindowIcon(QIcon(":/icons/lxqt-clip.png"));
    msgBox.setIconPixmap (QPixmap(":/icons/lxqt-clip.png"));
    msgBox.setWindowTitle(tr("About lxqt-clip"));
    msgBox.setText(QString("<h1>lxqt-clip</strong> %1</h1>").arg(qApp->applicationVersion()));
    msgBox.setInformativeText(tr("Lightweight, clipboard history applet.<p>"
                                  "(c)<ul><li>2010-2016&nbsp;Petr&nbsp;Vanek&nbsp;&lt;petr@yarpen.cz&gt;</li>"
                                  "<li>2026- LXQt team <a href=\"https://lxqt-project.org\">lxqt-project.org</a></li></ul>"
                                  "<a href=\"https://github.com/lxqt/lxqt-clip/\">https://github.com/lxqt/lxqt-clip/</a>"
                                  "<p>"
                                  "Support for global keyboard shortcut: %1").arg(globalKeySupport));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void Systray::systray_activated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
        shortcut_activated();
}

void Systray::clear_history()
{
    if (!Preferences::Instance()->confirmOnClear())
    {
        m_model->clearHistory();
        return;
    }

    int ret = QMessageBox::question(nullptr, tr("Confirm"), tr("Are you sure that you want to clear your clipboard history?"),
                                    QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
        m_model->clearHistory();
}

} // namespace
